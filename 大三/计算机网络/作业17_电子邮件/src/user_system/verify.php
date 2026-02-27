<?php
require_once 'includes/functions.php';

$username = $_GET['user'] ?? '';
$code = $_GET['code'] ?? '';

if (empty($username) || empty($code)) {
    die('无效的激活链接');
}

$conn = getDBConnection();

// 查找用户
$stmt = $conn->prepare("
    SELECT id, activation_code, expires_at 
    FROM users 
    WHERE username = ? AND is_active = 0
");
$stmt->bind_param("s", $username);
$stmt->execute();
$result = $stmt->get_result();

if ($result->num_rows === 0) {
    $message = "用户不存在或已激活";
} else {
    $user = $result->fetch_assoc();
    
    // 检查激活码是否匹配
    if ($user['activation_code'] === $code) {
        // 检查是否过期
        if (strtotime($user['expires_at']) > time()) {
            // 激活用户
            $update_stmt = $conn->prepare("
                UPDATE users 
                SET is_active = 1, activation_code = NULL 
                WHERE id = ?
            ");
            $update_stmt->bind_param("i", $user['id']);
            
            if ($update_stmt->execute()) {
                $message = "账户激活成功！您现在可以登录了。";
            } else {
                $message = "激活失败，请重试";
            }
            $update_stmt->close();
        } else {
            // 链接已过期，删除该用户记录
            $delete_stmt = $conn->prepare("DELETE FROM users WHERE id = ?");
            $delete_stmt->bind_param("i", $user['id']);
            $delete_stmt->execute();
            $delete_stmt->close();
            
            $message = "激活链接已过期！该用户名已被释放。";
        }
    } else {
        $message = "无效的激活码";
    }
}

$stmt->close();
$conn->close();
?>

<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>账户激活</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
        }
        .container {
            background: white;
            padding: 40px;
            border-radius: 10px;
            box-shadow: 0 0 20px rgba(0,0,0,0.1);
            text-align: center;
            max-width: 500px;
        }
        .success {
            color: #4CAF50;
            font-size: 24px;
            margin-bottom: 20px;
        }
        .error {
            color: #f44336;
            font-size: 24px;
            margin-bottom: 20px;
        }
        .message {
            margin-bottom: 30px;
            font-size: 16px;
            line-height: 1.6;
        }
        .button {
            display: inline-block;
            padding: 10px 30px;
            background-color: #4CAF50;
            color: white;
            text-decoration: none;
            border-radius: 5px;
            font-size: 16px;
        }
        .button:hover {
            background-color: #45a049;
        }
    </style>
</head>
<body>
    <div class="container">
        <?php if (strpos($message, '成功') !== false): ?>
            <div class="success">✅ 激活成功</div>
        <?php else: ?>
            <div class="error">❌ 激活失败</div>
        <?php endif; ?>
        
        <div class="message"><?php echo htmlspecialchars($message); ?></div>
        
        <?php if (strpos($message, '成功') !== false): ?>
            <a href="login.php" class="button">立即登录</a>
        <?php else: ?>
            <a href="register.php" class="button">重新注册</a>
        <?php endif; ?>
    </div>
</body>
</html>