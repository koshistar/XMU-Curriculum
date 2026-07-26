%% ------ 参数配置 ------
TEXTURE_PATH = 'texture_parmesan.png';
TARGET_PATH  = 'target_face.png';

PATCH_SIZE = 35;
OVERLAP    = 10;
TOLERANCE  = 0.1;
OUT_SIZE   = [200, 200];
ALPHA      = 0.1;

%% ------ 如无图像文件，自动生成演示用图像 ------
if ~isfile(TEXTURE_PATH)
    fprintf('未找到纹理图像，生成演示用棋盘格纹理...');
    demo = zeros(64, 64, 3);
    for i = 1:8:57
        for j = 1:8:57
            ii = (i-1)/8 + 1;
            jj = (j-1)/8 + 1;
            if mod(ii + jj, 2) == 0
                demo(i:i+7, j:j+7, :) = 0.8;
                demo(i:i+7, j:j+7, 1) = 0.8;
                demo(i:i+7, j:j+7, 2) = 0.3;
                demo(i:i+7, j:j+7, 3) = 0.1;
            else
                demo(i:i+7, j:j+7, 1) = 0.2;
                demo(i:i+7, j:j+7, 2) = 0.6;
                demo(i:i+7, j:j+7, 3) = 0.9;
            end
        end
    end
    imwrite(demo, TEXTURE_PATH);
    fprintf('  已生成: %s', TEXTURE_PATH);
end

if ~isfile(TARGET_PATH)
    fprintf('未找到目标图像，生成演示用渐变目标图...');
    [X, ~] = meshgrid(1:200, 1:200);
    demo_t = zeros(200, 200, 3);
    demo_t(:, :, 1) = X / 200.0;
    demo_t(:, :, 2) = 0.5;
    demo_t(:, :, 3) = 1.0 - X / 200.0;
    imwrite(demo_t, TARGET_PATH);
    fprintf('  已生成: %s', TARGET_PATH);
end

fprintf('加载纹理样本图...');
texture = im2double(imread(TEXTURE_PATH));
if size(texture, 3) == 1
    texture = repmat(texture, [1, 1, 3]);
end
[Ht, Wt, Ct] = size(texture);
out_H = OUT_SIZE(1);
out_W = OUT_SIZE(2);

fprintf('随机铺块...');
result_random = randomQuilt(texture, out_H, out_W, PATCH_SIZE, OVERLAP);
imwrite(result_random, 'output_random.png');

fprintf('最优块选择 + 最小割缝合...');
result_quilt = quiltCut(texture, out_H, out_W, PATCH_SIZE, OVERLAP, TOLERANCE);
imwrite(result_quilt, 'output_quilted.png');

fprintf('纹理迁移...');
target = im2double(imread(TARGET_PATH));
if size(target, 3) == 1
    target = repmat(target, [1, 1, 3]);
end
target_resized = imresize(target, [out_H, out_W]);
% 迭代
iterations=3;
alphas=linspace(0.3,0.8,iterations);
patch_sizes=PATCH_SIZE.*linspace(1.0,0.6,iterations);
patch_sizes=round(patch_sizes);
patch_sizes(patch_sizes<6)=6;
result_transfer = [];
for iter=1:iterations
    curr_alpha=alphas(iter);
    curr_patch=patch_sizes(iter);
    curr_overlap=round(curr_patch/3);
    result_transfer = textureTransfer(texture, target_resized, curr_patch, curr_overlap, curr_alpha, TOLERANCE, result_transfer);
end
imwrite(result_transfer, 'output_transfer.png');

fprintf('可视化结果...');
figure('Name', 'Image Quilting Results', 'Color', 'w', 'Position', [100, 100, 1300, 280]);
subplot(1,5,1); imshow(texture);
title('纹理样本', 'FontSize', 12);
subplot(1,5,2); imshow(result_random);
title('随机铺块', 'FontSize', 12);
subplot(1,5,3); imshow(result_quilt);
title('Quilting', 'FontSize', 12);
subplot(1,5,4); imshow(target_resized);
title('目标图像', 'FontSize', 12);
subplot(1,5,5); imshow(result_transfer);
title('纹理迁移', 'FontSize', 12);