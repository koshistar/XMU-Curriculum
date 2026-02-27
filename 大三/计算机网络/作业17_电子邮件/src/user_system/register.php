<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>用户注册</title>
    <link rel="stylesheet" href="css/style.css">
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            margin: 0;
            padding: 20px;
        }
        .container {
            max-width: 400px;
            margin: 0 auto;
            background: white;
            padding: 30px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0,0,0,0.1);
        }
        h2 {
            text-align: center;
            color: #333;
        }
        .form-group {
            margin-bottom: 20px;
        }
        label {
            display: block;
            margin-bottom: 5px;
            color: #666;
        }
        input[type="text"],
        input[type="password"],
        input[type="email"] {
            width: 100%;
            padding: 10px;
            border: 1px solid #ddd;
            border-radius: 5px;
            font-size: 16px;
        }
        button {
            width: 100%;
            padding: 12px;
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 5px;
            font-size: 16px;
            cursor: pointer;
        }
        button:hover {
            background-color: #45a049;
        }
        .message {
            padding: 10px;
            border-radius: 5px;
            margin-bottom: 20px;
            text-align: center;
        }
        .success {
            background-color: #d4edda;
            color: #155724;
            border: 1px solid #c3e6cb;
        }
        .error {
            background-color: #f8d7da;
            color: #721c24;
            border: 1px solid #f5c6cb;
        }
        .login-link {
            text-align: center;
            margin-top: 20px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h2>用户注册</h2>
        
        <?php if (isset($_GET['success'])): ?>
            <div class="message success">
                注册成功！请检查您的邮箱并激活账户。
            </div>
        <?php endif; ?>
        
        <?php if (isset($_GET['error'])): ?>
            <div class="message error">
                <?php 
                $errors = [
                    'username_taken' => '用户名已被占用',
                    'email_taken' => '邮箱已被注册',
                    'mail_failed' => '邮件发送失败，请联系管理员',
                    'registration_failed' => '注册失败，请重试'
                ];
                echo $errors[$_GET['error']] ?? '发生未知错误';
                ?>
            </div>
        <?php endif; ?>
        
        <form action="process_register.php" method="POST" onsubmit="return validateForm()">
            <div class="form-group">
                <label for="username">昵称：</label>
                <input type="text" id="username" name="username" required 
                       placeholder="请输入昵称" onblur="checkUsername()">
                <small id="username-help" style="color:#666; font-size:12px;">1小时内不激活将释放该昵称</small>
                <div id="username-feedback" style="font-size:14px; margin-top:5px;"></div>
            </div>
            
            <div class="form-group">
                <label for="email">邮箱：</label>
                <input type="email" id="email" name="email" required 
                       placeholder="请输入邮箱">
            </div>
            
            <div class="form-group">
                <label for="password">口令：</label>
                <input type="password" id="password" name="password" required 
                       placeholder="请输入口令" minlength="6">
            </div>
            
            <div class="form-group">
                <label for="confirm_password">确认口令：</label>
                <input type="password" id="confirm_password" name="confirm_password" required 
                       placeholder="请再次输入口令">
            </div>
            
            <button type="submit">注册</button>
        </form>
        
        <div class="login-link">
            <p>已有账户？<a href="login.php">立即登录</a></p>
        </div>
    </div>
    
    <script>
        function checkUsername() {
            const username = document.getElementById('username').value;
            if (username.length < 3) return;
            
            fetch(`check_username.php?username=${encodeURIComponent(username)}`)
                .then(response => response.json())
                .then(data => {
                    const feedback = document.getElementById('username-feedback');
                    if (data.available) {
                        feedback.innerHTML = '<span style="color:green">✓ 用户名可用</span>';
                    } else {
                        feedback.innerHTML = '<span style="color:red">✗ 用户名不可用</span>';
                    }
                });
        }
        
        function validateForm() {
            const password = document.getElementById('password').value;
            const confirmPassword = document.getElementById('confirm_password').value;
            
            if (password !== confirmPassword) {
                alert('两次输入的口令不一致！');
                return false;
            }
            
            if (password.length < 6) {
                alert('口令长度至少6位！');
                return false;
            }
            
            return true;
        }
    </script>
</body>
</html>