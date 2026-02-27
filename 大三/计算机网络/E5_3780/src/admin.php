<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <title>许可证服务器监控面板</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            font-family: Arial, sans-serif;
        }
        .container {
            width: 90%;
            max-width: 1000px;
            margin: 30px auto;
        }
        .status-card {
            background: #f5f5f5;
            padding: 20px;
            border-radius: 8px;
            margin-bottom: 20px;
        }
        .status-card h2 {
            color: #333;
            margin-bottom: 15px;
        }
        .status-item {
            margin: 8px 0;
            font-size: 16px;
            color: #555;
        }
        table {
            width: 100%;
            border-collapse: collapse;
            background: white;
            border-radius: 8px;
            overflow: hidden;
            box-shadow: 0 0 10px rgba(0,0,0,0.1);
        }
        th, td {
            padding: 12px 15px;
            text-align: center;
            border-bottom: 1px solid #eee;
        }
        th {
            background: #2c3e50;
            color: white;
        }
        tr:hover {
            background: #f9f9f9;
        }
        .status-full {
            color: #e74c3c;
            font-weight: bold;
        }
        .status-available {
            color: #2ecc71;
            font-weight: bold;
        }
        .empty-data {
            text-align: center;
            padding: 30px;
            color: #777;
        }
    </style>
</head>
<body>
    <div class="container">
        <!-- 服务器状态 -->
        <div class="status-card">
            <h2>许可证服务器状态</h2>
            <?php
            $jsonFile = 'license_data.json';
            $data = [];
            
            // 读取JSON文件
            if (file_exists($jsonFile)) {
                $jsonContent = file_get_contents($jsonFile);
                $data = json_decode($jsonContent, true);
            }
            ?>

            <?php if (!empty($data)): ?>
                <div class="status-item">服务器启动时间：<?php echo htmlspecialchars($data['server_start_time']); ?></div>
                <div class="status-item">数据最后更新：<?php echo htmlspecialchars($data['update_time']); ?></div>
                <div class="status-item">单许可证最大用户数：<?php echo $data['max_license']; ?></div>
            <?php else: ?>
                <div class="status-item">服务器未运行或数据文件不存在</div>
            <?php endif; ?>
        </div>

        <!-- 许可证列表 -->
        <table>
            <thead>
                <tr>
                    <th>许可证序列号</th>
                    <th>当前在线用户数</th>
                    <th>剩余许可数</th>
                    <th>许可证状态</th>
                </tr>
            </thead>
            <tbody>
                <?php if (!empty($data['licenses']) && count($data['licenses']) > 0): ?>
                    <?php foreach ($data['licenses'] as $license): ?>
                        <tr>
                            <td><?php echo htmlspecialchars($license['serial']); ?></td>
                            <td><?php echo $license['user_count']; ?></td>
                            <td><?php echo $data['max_license'] - $license['user_count']; ?></td>
                            <td>
                                <?php if ($license['user_count'] >= $data['max_license']): ?>
                                    <span class="status-full">已满</span>
                                <?php else: ?>
                                    <span class="status-available">可用</span>
                                <?php endif; ?>
                            </td>
                        </tr>
                    <?php endforeach; ?>
                <?php else: ?>
                    <tr>
                        <td colspan="4" class="empty-data">暂无许可证数据</td>
                    </tr>
                <?php endif; ?>
            </tbody>
        </table>
    </div>
</body>
</html>