<?php
require_once 'includes/functions.php';
require_once 'includes/mailer.php';

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $username = trim($_POST['username']);
    $email = trim($_POST['email']);
    $password = $_POST['password'];
    
    // 验证输入
    if (empty($username) || empty($email) || empty($password)) {
        header('Location: register.php?error=empty_fields');
        exit;
    }
    
    // 检查邮箱格式
    if (!filter_var($email, FILTER_VALIDATE_EMAIL)) {
        header('Location: register.php?error=invalid_email');
        exit;
    }
    
    // 注册用户
    $result = registerUser($username, $email, $password);
    
    if ($result['success']) {
        // 发送激活邮件
        if (sendActivationEmail($email, $username, $result['activation_code'])) {
            header('Location: register.php?success=1');
        } else {
            // 邮件发送失败，删除用户记录
            $conn = getDBConnection();
            $stmt = $conn->prepare("DELETE FROM users WHERE username = ?");
            $stmt->bind_param("s", $username);
            $stmt->execute();
            $stmt->close();
            $conn->close();
            
            header('Location: register.php?error=mail_failed');
        }
    } else {
        header('Location: register.php?error=' . urlencode($result['message']));
    }
} else {
    header('Location: register.php');
}
?>