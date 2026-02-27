<?php
use PHPMailer\PHPMailer\PHPMailer;
use PHPMailer\PHPMailer\Exception;

require_once __DIR__ . '/../phpmailer/src/Exception.php';
require_once __DIR__ . '/../phpmailer/src/PHPMailer.php';
require_once __DIR__ . '/../phpmailer/src/SMTP.php';

function sendActivationEmail($email, $username, $activation_code) {
    $mail = new PHPMailer(true);
    
    try {
        // 服务器配置
        $mail->isSMTP();
        $mail->Host = SMTP_HOST;
        $mail->SMTPAuth = true;
        $mail->Username = SMTP_USER;
        $mail->Password = SMTP_PASS;
        $mail->SMTPSecure = PHPMailer::ENCRYPTION_SMTPS;
        $mail->Port = SMTP_PORT;
        
        // 收件人
        $mail->setFrom(FROM_EMAIL, FROM_NAME);
        $mail->addAddress($email, $username);
        
        // 内容
        $mail->isHTML(true);
        $mail->Subject = '账户激活 - 用户注册系统';
        
        $activation_link = SITE_URL . "verify.php?user=" . urlencode($username) . "&code=" . $activation_code;
        
        $mail->Body = "
        <html>
        <body style='font-family: Arial, sans-serif;'>
            <h2>欢迎注册用户系统</h2>
            <p>亲爱的 {$username}，</p>
            <p>感谢您注册我们的系统！请点击以下链接激活您的账户：</p>
            <p><a href='{$activation_link}' style='background-color: #4CAF50; color: white; padding: 14px 20px; text-decoration: none; display: inline-block; border-radius: 5px;'>激活账户</a></p>
            <p>或者复制以下链接到浏览器：<br>{$activation_link}</p>
            <p><strong>注意：此链接将在1小时后失效！</strong></p>
            <p>如果这不是您注册的账户，请忽略此邮件。</p>
            <hr>
            <p style='color: #666; font-size: 12px;'>系统自动发送，请勿回复。</p>
        </body>
        </html>
        ";
        
        $mail->AltBody = "欢迎注册用户系统！\n\n请点击链接激活账户：{$activation_link}\n\n此链接将在1小时后失效！";
        
        $mail->send();
        return true;
    } catch (Exception $e) {
        error_log("邮件发送失败: " . $mail->ErrorInfo);
        return false;
    }
}
?>