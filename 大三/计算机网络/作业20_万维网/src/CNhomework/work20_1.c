#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<Windows.h>
#include<time.h>
#include<io.h>
#include<sys/stat.h>
#pragma comment(lib,"ws2_32.lib")
#define PORT 8080
#define BUFFER_SIZE 4096
#define MAX_PATH_LENGTH 256
#define MAX_CONNECTIONS 10
#define HTTP_OK 200
#define HTTP_NOT_FOUND 404
#define HTTP_INTERNAL_ERROR 500
#define HTTP_BAD_REQUEST 400
#define HTTP_FORBIDDEN 403
typedef struct
{
	char* ext;
	char* mime_type;
}MimeType;

MimeType mime_types[] = {
    {".html", "text/html"},
    {".htm", "text/html"},
    {".css", "text/css"},
    {".js", "application/javascript"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".png", "image/png"},
    {".gif", "image/gif"},
    {".ico", "image/x-icon"},
    {".txt", "text/plain"},
    {".json", "application/json"},
    {".xml", "application/xml"},
    {".pdf", "application/pdf"},
    {".zip", "application/zip"},
    {NULL, "application/octet-stream"}
};
volatile BOOL g_bQuit = FALSE;
BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType)
{
    if (dwCtrlType == CTRL_C_EVENT)
    {
        printf("\n[INFO] 接收到退出信号，正在关闭服务器...\n");
        g_bQuit = TRUE;
        return TRUE;
    }
    return FALSE;
}
char* get_mime_type(const char* filename)
{
    const char* dot = strchr(filename, '.');
    if (dot == NULL)
    {
        return "application/octet-stream";
    }
    for (int i = 0; mime_types[i].ext != NULL; i++)
    {
        if (_stricmp(dot, mime_types[i].ext) == 0)
        {
            return mime_types[i].mime_type;
        }
    }
    return "application/octet-stream";
}
int file_exists(const char* filename)
{
    FILE* file = fopen(filename, "rb");
    if (file)
    {
        fclose(file);
        return 1;
    }
    return 0;
}
long get_file_size(const char* filename)
{
    FILE* file = fopen(filename, "rb");
    if (!file)
        return 0;
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    fclose(file);
    return size;
}
void send_headers(SOCKET client_socket, int status_code, const char* content_type, long content_length)
{
    char headers[BUFFER_SIZE] = { 0 };
    const char* status_text;
    switch (status_code)
    {
    case HTTP_OK:status_text = "OK";
        break;
    case HTTP_NOT_FOUND:status_text = "Not Found"; break;
    case HTTP_INTERNAL_ERROR:status_text = "Internal Server Error"; break;
    case HTTP_BAD_REQUEST:status_text = "Bad Request"; break;
    case HTTP_FORBIDDEN:status_text = "Forbidden"; break;
    default:status_text = "Unknown"; break;
    }
    time_t now = time(NULL);
    struct tm* tm = gmtime(&now);
    char date_buf[100];
    strftime(date_buf, sizeof(date_buf), "%a,%d %b %Y %H:%M:%S GMT", tm);
    int ret = snprintf(headers, sizeof(headers), "HTTP/1.1 %d %s\r\n""Server: Work20/1.0\r\n""Date: %s\r\n""Content-Type: %s\r\n""Content-Length: %ld\r\n""Connection: close\r\n""\r\n", status_code, status_text, date_buf, content_type, content_length);
    if (ret<= 0)
    {
        printf("[ERROR] response connect failed");
        return;
    }
    int send_len = send(client_socket, headers, (int)strlen(headers), 0);
    if (send_len == SOCKET_ERROR)
    {
        printf("[ERROR] send response head failed: %d\n", WSAGetLastError());
    }
}
void send_error_page(SOCKET client_socket, int status_code, const char* message)
{
    char error_page[BUFFER_SIZE] = { 0 };
    snprintf(error_page, sizeof(error_page) - 1,
        "<!DOCTYPE html>\n"
        "<html lang=\"en\">\n"
        "<head>\n"
        "   <meta charset=\"UTF-8\">\n"
        "   <title>Error %d</title>\n"
        "   <style>\n"
        "       body { font-family: Arial, sans-serif; margin: 40px; background-color: #f0f0f0; }\n"
        "       .container { max-width: 600px; margin: 0 auto; background-color: white; padding: 30px; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }\n"
        "       h1 { color: #333; border-bottom: 2px solid #ff4444; padding-bottom: 10px; }\n"
        "       .error { color: #ff4444; font-weight: bold; }\n"
        "       a { color: #2196F3; text-decoration: none; }\n"
        "       a:hover { text-decoration: underline; }\n"
        "   </style>\n"
        "</head>\n"
        "<body>\n"
        "   <div class=\"container\">\n"
        "       <h1>Error %d: %s</h1>\n"
        "       <p class=\"error\">%s</p>\n"
        "       <p><a href=\"/\">Back to Home</a></p>\n"
        "   </div>\n"
        "</body>\n"
        "</html>", status_code, status_code, message, message);
    send_headers(client_socket, status_code, "text/html", strlen(error_page));
    send(client_socket, error_page, (int)strlen(error_page), 0);
}
void send_file(SOCKET client_socket, const char* filename)
{
    char buffer[BUFFER_SIZE] = { 0 };
    size_t bytes_read;
    long file_size = get_file_size(filename);
    FILE* file = fopen(filename, "rb");
    if (file == NULL)
    {
        send_error_page(client_socket, HTTP_NOT_FOUND, "File not found");
        return;
    }
    char* content_type = get_mime_type(filename);
    send_headers(client_socket, HTTP_OK, content_type, file_size);
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0 && !g_bQuit)
    {
        send(client_socket, buffer, (int)bytes_read, 0);
    }
    fclose(file);
}
void handle_request(SOCKET client_socket, char* request)
{
    char method[16], path[MAX_PATH_LENGTH], protocol[16];
    if (sscanf(request, "%15s %255s %15s", method, path, protocol) != 3)
    {
        send_error_page(client_socket, HTTP_BAD_REQUEST, "Bad Request");
        return;
    }
    if (_stricmp(method, "GET") != 0)
    {
        send_error_page(client_socket, HTTP_BAD_REQUEST, "Only GET method is supported");
        return;
    }
    char filepath[MAX_PATH_LENGTH];
    if (strcmp(path, "/") == 0)
    {
        snprintf(filepath, sizeof(filepath) - 1, "www\\index.html");
    }
    else
    {
        if (strstr(path, "..") != NULL || strstr(path, "//") != NULL)
        {
            send_error_page(client_socket, HTTP_FORBIDDEN, "Forbidden");
            return;
        }
        char winpath[MAX_PATH_LENGTH];
        strncpy(winpath, path, sizeof(winpath) - 1);
        //winpath[sizeof(winpath) - 1] = '\0';
        for (char* p = winpath; *p; p++)
        {
            if (*p == '/')
            {
                *p = '\\';
            }
        }
        snprintf(filepath, sizeof(filepath) - 1, "www%s", winpath);
    }
    printf("[INFO] Request: %s %s -> Serving: %s\n", method, path, filepath);
    if (!file_exists(filepath))
    {
        send_error_page(client_socket, HTTP_NOT_FOUND, "File not found");
        return;
    }
    send_file(client_socket, filepath);
}
void create_sample_files()
{
    CreateDirectory("www", NULL);
    FILE* index = fopen("www\\index.html", "wb");
    if (index)
    {
        fprintf(index, "<!DOCTYPE html>\n");
        fprintf(index, "<html lang=\"en\">\n");
        fprintf(index, "<head>\n");
        fprintf(index, "    <meta charset=\"UTF-8\">\n");
        fprintf(index, "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
        fprintf(index, "    <title>Simple HTTP Server for Windows</title>\n");
        fprintf(index, "    <style>\n");
        fprintf(index, "        body { font-family: 'Segoe UI', Arial, sans-serif; margin: 40px; background: linear-gradient(135deg, #667eea 0%%, #764ba2 100%%); min-height: 100vh; }\n");
        fprintf(index, "        .container { max-width: 800px; margin: 0 auto; background-color: white; padding: 40px; border-radius: 15px; box-shadow: 0 20px 60px rgba(0,0,0,0.3); }\n");
        fprintf(index, "        h1 { color: #333; border-bottom: 3px solid #4CAF50; padding-bottom: 15px; margin-bottom: 30px; }\n");
        fprintf(index, "        .success { color: #4CAF50; font-weight: bold; font-size: 1.2em; background-color: #e8f5e9; padding: 10px; border-radius: 5px; }\n");
        fprintf(index, "        a { color: #2196F3; text-decoration: none; font-weight: 500; }\n");
        fprintf(index, "        a:hover { text-decoration: underline; }\n");
        fprintf(index, "        .links { margin: 25px 0; padding: 20px; background-color: #f5f5f5; border-radius: 8px; }\n");
        fprintf(index, "        .links h3 { margin-top: 0; }\n");
        fprintf(index, "        .card { background: white; border-radius: 10px; padding: 20px; margin: 20px 0; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }\n");
        fprintf(index, "        .footer { margin-top: 30px; padding-top: 20px; border-top: 1px solid #eee; color: #666; font-size: 0.9em; }\n");
        fprintf(index, "        .feature-grid { display: grid; grid-template-columns: repeat(auto-fill, minmax(200px, 1fr)); gap: 20px; margin: 20px 0; }\n");
        fprintf(index, "        .feature { background: #f8f9fa; padding: 15px; border-radius: 8px; border-left: 4px solid #4CAF50; }\n");
        fprintf(index, "    </style>\n");
        fprintf(index, "</head>\n");
        fprintf(index, "<body>\n");
        fprintf(index, "    <div class=\"container\">\n");
        fprintf(index, "        <h1> Simple HTTP Server for Windows</h1>\n");
        fprintf(index, "        <p class=\"success\"> Server is running successfully!</p>\n");
        fprintf(index, "        <p>This is a lightweight HTTP server written in C, specifically designed for Windows.</p>\n");

        fprintf(index, "        <div class=\"feature-grid\">\n");
        fprintf(index, "            <div class=\"feature\">\n");
        fprintf(index, "                <h3> File Serving</h3>\n");
        fprintf(index, "                <p>Serve static HTML, CSS, JavaScript, images, and more.</p>\n");
        fprintf(index, "            </div>\n");
        fprintf(index, "            <div class=\"feature\">\n");
        fprintf(index, "                <h3> Fast & Lightweight</h3>\n");
        fprintf(index, "                <p>No dependencies, minimal resource usage.</p>\n");
        fprintf(index, "            </div>\n");
        fprintf(index, "            <div class=\"feature\">\n");
        fprintf(index, "                <h3> Secure</h3>\n");
        fprintf(index, "                <p>Basic security measures against common attacks.</p>\n");
        fprintf(index, "            </div>\n");
        fprintf(index, "            <div class=\"feature\">\n");
        fprintf(index, "                <h3> Simple Configuration</h3>\n");
        fprintf(index, "                <p>Easy to modify and extend for your needs.</p>\n");
        fprintf(index, "            </div>\n");
        fprintf(index, "        </div>\n");

        fprintf(index, "        <div class=\"links\">\n");
        fprintf(index, "            <h3> Test Pages:</h3>\n");
        fprintf(index, "            <ul>\n");
        fprintf(index, "                <li><a href=\"/index.html\"> Home Page</a></li>\n");
        fprintf(index, "                <li><a href=\"/about.html\"> About Page</a></li>\n");
        fprintf(index, "                <li><a href=\"/test.txt\"> Text File</a></li>\n");
        fprintf(index, "                <li><a href=\"/style.css\"> CSS File</a></li>\n");
        fprintf(index, "                <li><a href=\"/nonexistent.html\"> Non-existent Page (404)</a></li>\n");
        fprintf(index, "            </ul>\n");
        fprintf(index, "        </div>\n");

        fprintf(index, "        <div class=\"card\">\n");
        fprintf(index, "            <h3>Server Information</h3>\n");
        fprintf(index, "            <p><strong>Port:</strong> %d</p>\n", PORT);
        fprintf(index, "            <p><strong>Web Root:</strong> ./www/</p>\n");
        fprintf(index, "            <p><strong>Access URL:</strong> <a href=\"http://localhost:%d\">http://localhost:%d</a></p>\n", PORT, PORT);
        fprintf(index, "            <p><strong>Platform:</strong> Windows</p>\n");
        fprintf(index, "        </div>\n");

        fprintf(index, "        <div class=\"footer\">\n");
        fprintf(index, "            <p>Server written in pure C using Winsock API. Press Ctrl+C in the console to stop the server.</p>\n");
        fprintf(index, "        </div>\n");
        fprintf(index, "    </div>\n");
        fprintf(index, "</body>\n");
        fprintf(index, "</html>\n");
        fclose(index);
    }
    FILE* about = fopen("www\\about.html", "w");
    if (about) {
        fprintf(about, "<!DOCTYPE html>\n");
        fprintf(about, "<html lang=\"en\">\n");
        fprintf(about, "<head>\n");
        fprintf(about, "    <meta charset=\"UTF-8\">\n");
        fprintf(about, "    <title>About - Simple HTTP Server</title>\n");
        fprintf(about, "    <style>\n");
        fprintf(about, "        body { font-family: 'Segoe UI', Arial, sans-serif; margin: 40px; background-color: #f5f5f5; }\n");
        fprintf(about, "        .container { max-width: 700px; margin: 0 auto; background-color: white; padding: 30px; border-radius: 10px; box-shadow: 0 10px 30px rgba(0,0,0,0.1); }\n");
        fprintf(about, "        h1 { color: #333; border-bottom: 2px solid #4CAF50; padding-bottom: 10px; }\n");
        fprintf(about, "        .highlight { background-color: #e8f5e9; padding: 15px; border-radius: 5px; margin: 20px 0; }\n");
        fprintf(about, "        a { color: #2196F3; text-decoration: none; }\n");
        fprintf(about, "        a:hover { text-decoration: underline; }\n");
        fprintf(about, "        code { background-color: #f1f1f1; padding: 2px 6px; border-radius: 3px; font-family: Consolas, monospace; }\n");
        fprintf(about, "    </style>\n");
        fprintf(about, "</head>\n");
        fprintf(about, "<body>\n");
        fprintf(about, "    <div class=\"container\">\n");
        fprintf(about, "        <h1>About This Server</h1>\n");
        fprintf(about, "        <p>This is a simple HTTP server implemented in C specifically for Windows.</p>\n");

        fprintf(about, "        <div class=\"highlight\">\n");
        fprintf(about, "            <h3>Key Features:</h3>\n");
        fprintf(about, "            <ul>\n");
        fprintf(about, "                <li>Handles HTTP/1.1 GET requests</li>\n");
        fprintf(about, "                <li>Serves static files with proper MIME types</li>\n");
        fprintf(about, "                <li>Error handling (404, 500, 403)</li>\n");
        fprintf(about, "                <li>Windows-specific optimizations</li>\n");
        fprintf(about, "                <li>No external dependencies</li>\n");
        fprintf(about, "            </ul>\n");
        fprintf(about, "        </div>\n");

        fprintf(about, "        <h3>Technical Details</h3>\n");
        fprintf(about, "        <p>This server uses the Winsock API for network communication and standard C library for file I/O.</p>\n");

        fprintf(about, "        <h3>Building and Running</h3>\n");
        fprintf(about, "        <p>Compile with:</p>\n");
        fprintf(about, "        <code>cl http_server.c /Fe:http_server.exe ws2_32.lib</code>\n");
        fprintf(about, "        <p>Or in Visual Studio: Link with <code>ws2_32.lib</code></p>\n");

        fprintf(about, "        <p><a href=\"/\">← Back to Home</a></p>\n");
        fprintf(about, "    </div>\n");
        fprintf(about, "</body>\n");
        fprintf(about, "</html>\n");
        fclose(about);
    }
    FILE* txt = fopen("www\\test.txt", "w");
    if (txt) {
        fprintf(txt, "This is a plain text file served by the Windows HTTP server.\n");
        fprintf(txt, "=============================================================\n");
        fprintf(txt, "\n");
        fprintf(txt, "Server Information:\n");
        fprintf(txt, "- Platform: Windows\n");
        fprintf(txt, "- Port: %d\n", PORT);
        fprintf(txt, "- Protocol: HTTP/1.1\n");
        fprintf(txt, "\n");
        fprintf(txt, "You can use this server to serve any type of static content.\n");
        fclose(txt);
    }
    FILE* css = fopen("www\\style.css", "w");
    if (css) {
        fprintf(css, "/* Example CSS file for Windows HTTP Server */\n");
        fprintf(css, "* { margin: 0; padding: 0; box-sizing: border-box; }\n");
        fprintf(css, "\n");
        fprintf(css, "body {\n");
        fprintf(css, "    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;\n");
        fprintf(css, "    line-height: 1.6;\n");
        fprintf(css, "    color: #333;\n");
        fprintf(css, "}\n");
        fprintf(css, "\n");
        fprintf(css, ".button {\n");
        fprintf(css, "    display: inline-block;\n");
        fprintf(css, "    background-color: #4CAF50;\n");
        fprintf(css, "    color: white;\n");
        fprintf(css, "    padding: 10px 20px;\n");
        fprintf(css, "    text-decoration: none;\n");
        fprintf(css, "    border-radius: 5px;\n");
        fprintf(css, "    transition: background-color 0.3s;\n");
        fprintf(css, "}\n");
        fprintf(css, "\n");
        fprintf(css, ".button:hover {\n");
        fprintf(css, "    background-color: #45a049;\n");
        fprintf(css, "}\n");
        fclose(css);
    }

    printf("[INFO] Created sample files in 'www' directory\n");
}
char* get_client_address(struct sockaddr_in* client_addr)
{
    static char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr->sin_addr), client_ip, INET_ADDRSTRLEN);
    return client_ip;
}
void cleanup(SOCKET server_socket)
{
    if (server_socket != INVALID_SOCKET)
    {
        closesocket(server_socket);
    }
    WSACleanup();
}
int main()
{
    WSADATA wsaData;
    SOCKET server_socket = INVALID_SOCKET;
    SOCKET client_socket = INVALID_SOCKET;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    int result;
    if (!SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE))
    {
        printf("[WARN] set signal handle failed: %d\n", GetLastError());
    }
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        printf("[ERROR] WSAStartup failed: %d\n", result);
        return 1;
    }
    create_sample_files();
    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET)
    {
        printf("[ERROR] Socket creation failed: %d\n", WSAGetLastError());
        cleanup(server_socket);
        return 1;
    }
    int opt = 1;
    result = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
    if (result == SOCKET_ERROR)
    {
        printf("[ERROR] Setsocket failed: %d\n", WSAGetLastError());
        cleanup(server_socket);
        return 1;
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    result = bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (result == SOCKET_ERROR)
    {
        printf("[ERROR] Bind failed: %d\n", WSAGetLastError());
        cleanup(server_socket);
        return 1;
    }
    result = listen(server_socket, MAX_CONNECTIONS);
    if (result == SOCKET_ERROR)
    {
        printf("[ERROR] Listen failed: %d\n", WSAGetLastError());
        cleanup(server_socket);
        return 1;
    }
    char hostname[256];
    char ip_address[256];
    struct hostent* host_entry;
    gethostname(hostname, sizeof(hostname));
    host_entry = gethostbyname(hostname);
    printf("[INFO] Server started successfully!\n");
    printf("[INFO] Port: %d\n", PORT);
    printf("[INFO] Web root: ./www/\n");
    printf("[INFO] Access URLs:\n");
    printf("    http://localhost:%d\n", PORT);
    if (host_entry != NULL && host_entry->h_addr_list[0] != NULL)
    {
        inet_ntop(AF_INET, host_entry->h_addr_list[0], ip_address, sizeof(ip_address));
        printf("    http://%s:%d\n", ip_address, PORT);
    }
    while (!g_bQuit)
    {
        client_addr_len = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket == INVALID_SOCKET)
        {
            if (g_bQuit)
                break;
            printf("[ERROR] Accept failed: %d\n", WSAGetLastError());
            continue;
        }
        printf("[INFO] Connection from: %s:%d\n", get_client_address(&client_addr), ntohs(client_addr.sin_port));
        memset(buffer, 0, sizeof(buffer));
        result = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (result > 0)
        {
            buffer[result] = '\0';
            if (strstr(buffer, "HTTP") != NULL || strstr(buffer, "GET") != NULL)
            {
                handle_request(client_socket, buffer);
            }
            else
            {
                printf("[WARN] Non-HTTP request received\n");
                send_error_page(client_socket, HTTP_BAD_REQUEST, "Non-HTTP request");
            }
        }
        else if (result == 0)
        {
            printf("[INFO] Connection closed by client\n");
        }
        else
        {
            if(!g_bQuit)
                printf("[ERROR] recv failed: %d\n", WSAGetLastError());
        }
        closesocket(client_socket);
        client_socket = INVALID_SOCKET;
    }
    cleanup(server_socket);
    return 0;
}