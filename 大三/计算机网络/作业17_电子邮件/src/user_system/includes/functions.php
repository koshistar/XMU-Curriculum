<?php
session_start();
require_once __DIR__ . '/../config/database.php';

// 生成激活码
function generateActivationCode($username) {
    $salt = 'YOUR_SECRET_SALT';  // 修改为密钥
    return hash('sha256', $username . time() . $salt);
}

// 清理过期未激活用户
function cleanupExpiredUsers() {
    $conn = getDBConnection();
    $stmt = $conn->prepare("DELETE FROM users WHERE is_active = 0 AND expires_at < NOW()");
    $stmt->execute();
    $stmt->close();
    $conn->close();
}

// 检查用户名是否可用
function isUsernameAvailable($username) {
    cleanupExpiredUsers();  // 先清理过期用户
    
    $conn = getDBConnection();
    $stmt = $conn->prepare("SELECT id FROM users WHERE username = ? AND (is_active = 1 OR expires_at > NOW())");
    $stmt->bind_param("s", $username);
    $stmt->execute();
    $stmt->store_result();
    $available = ($stmt->num_rows === 0);
    
    $stmt->close();
    $conn->close();
    
    return $available;
}

// 用户注册
function registerUser($username, $email, $password) {
    if (!isUsernameAvailable($username)) {
        return ['success' => false, 'message' => '用户名已被占用'];
    }
    
    $conn = getDBConnection();
    
    // 生成激活信息
    $activation_code = generateActivationCode($username);
    $expires_at = date('Y-m-d H:i:s', strtotime('+1 hour'));
    $hashed_password = password_hash($password, PASSWORD_DEFAULT);
    
    $stmt = $conn->prepare("INSERT INTO users (username, email, password, activation_code, expires_at) VALUES (?, ?, ?, ?, ?)");
    $stmt->bind_param("sssss", $username, $email, $hashed_password, $activation_code, $expires_at);
    
    if ($stmt->execute()) {
        $stmt->close();
        $conn->close();
        return ['success' => true, 'activation_code' => $activation_code, 'username' => $username];
    } else {
        $error = $stmt->error;
        $stmt->close();
        $conn->close();
        return ['success' => false, 'message' => '注册失败: ' . $error];
    }
}
?>