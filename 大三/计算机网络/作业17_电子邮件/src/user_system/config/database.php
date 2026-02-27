<?php
// 数据库配置
define('DB_HOST', 'localhost');
define('DB_USER', 'root');
define('DB_PASS', '0d000721');
define('DB_NAME', 'user_system');

// 邮件配置（使用SMTP）
define('SMTP_HOST', 'smtp.qq.com');  // 根据邮箱服务商修改
define('SMTP_PORT', 465);
define('SMTP_USER', '188645751@qq.com');
define('SMTP_PASS', '0d000721');
define('FROM_EMAIL', '18864751@qq.com');
define('FROM_NAME', '用户注册系统');

// 系统配置
define('ACTIVATION_EXPIRE_HOURS', 1);  // 激活链接有效期（小时）
define('SITE_URL', 'http://127.0.0.1/user_system/');

// 创建数据库连接
function getDBConnection() {
    $conn = new mysqli(DB_HOST, DB_USER, DB_PASS, DB_NAME);
    
    if ($conn->connect_error) {
        die("数据库连接失败: " . $conn->connect_error);
    }
    
    $conn->set_charset("utf8mb4");
    return $conn;
}
?>