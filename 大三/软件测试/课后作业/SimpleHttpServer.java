import com.sun.net.httpserver.HttpServer;
import com.sun.net.httpserver.HttpHandler;
import com.sun.net.httpserver.HttpExchange;
import java.io.*;
import java.net.InetSocketAddress;
import java.util.*;

/**
 *  Java HTTP 服务器
 * 提供三个接口：
 *   GET /hello       — 简单字符串返回（轻量接口）
 *   GET /compute     — CPU密集型：计算斐波那契数列（重量接口）
 *   GET /data        — 模拟数据库查询（I/O延迟接口）
 */
public class SimpleHttpServer {

    public static void main(String[] args) throws Exception {
        int port = 8088;
        HttpServer server = HttpServer.create(new InetSocketAddress(port), 0);

        server.createContext("/hello",   new HelloHandler());
        server.createContext("/compute", new ComputeHandler());
        server.createContext("/data",    new DataHandler());

        server.setExecutor(java.util.concurrent.Executors.newFixedThreadPool(20));
        server.start();
        System.out.println("[SimpleHttpServer] Listening on port " + port);
        System.out.println("  GET http://localhost:" + port + "/hello");
        System.out.println("  GET http://localhost:" + port + "/compute");
        System.out.println("  GET http://localhost:" + port + "/data");
    }

    //  接口1：轻量Hello接口
    static class HelloHandler implements HttpHandler {
        @Override
        public void handle(HttpExchange exchange) throws IOException {
            String response = "{\"status\":\"ok\",\"message\":\"Hello, JMeter!\",\"timestamp\":" + System.currentTimeMillis() + "}";
            sendResponse(exchange, 200, response);
        }
    }

    // 接口2：CPU密集型（斐波那契 n=35）
    static class ComputeHandler implements HttpHandler {
        private long fibonacci(int n) {
            if (n <= 1) return n;
            return fibonacci(n - 1) + fibonacci(n - 2);
        }
        @Override
        public void handle(HttpExchange exchange) throws IOException {
            long result = fibonacci(35);
            String response = "{\"status\":\"ok\",\"fib35\":" + result + ",\"timestamp\":" + System.currentTimeMillis() + "}";
            sendResponse(exchange, 200, response);
        }
    }

    // 接口3：模拟数据库I/O（随机延迟10~50ms）
    static class DataHandler implements HttpHandler {
        private static final Random rand = new Random();
        @Override
        public void handle(HttpExchange exchange) throws IOException {
            try {
                // 模拟数据库查询延迟
                Thread.sleep(10 + rand.nextInt(40));
            } catch (InterruptedException ignored) {}
            String response = "{\"status\":\"ok\",\"rows\":42,\"query\":\"SELECT * FROM users LIMIT 10\",\"timestamp\":" + System.currentTimeMillis() + "}";
            sendResponse(exchange, 200, response);
        }
    }

    // 通用响应发送 
    static void sendResponse(HttpExchange exchange, int code, String body) throws IOException {
        exchange.getResponseHeaders().set("Content-Type", "application/json");
        byte[] bytes = body.getBytes("UTF-8");
        exchange.sendResponseHeaders(code, bytes.length);
        try (OutputStream os = exchange.getResponseBody()) {
            os.write(bytes);
        }
    }
}
