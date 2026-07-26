function output = randomQuilt(texture, out_H, out_W, patch_size, overlap)
    %% 随机铺块
    %  texture    : 纹理样本图 (H, W, 3) double [0,1]
    %  out_H, out_W : 输出图像高、宽
    %  patch_size: patch 边长（像素）
    %  overlap   : 相邻 patch 重叠宽度（像素）
    %  output    : 合成结果 (out_H, out_W, 3)

    [H, W, C] = size(texture);
    output = zeros(out_H, out_W, C);
    step = patch_size - overlap;

    count = 0;

    for y = 1:step:out_H
        for x = 1:step:out_W
            count = count + 1;

            % 随机采样一个 patch
            ty = randi(H - patch_size + 1);
            tx = randi(W - patch_size + 1);
            patch = texture(ty:ty+patch_size-1, tx:tx+patch_size-1, :);

            % 确定贴入范围
            y_end = min(y + patch_size - 1, out_H);
            x_end = min(x + patch_size - 1, out_W);
            ph = y_end - y + 1;
            pw = x_end - x + 1;

            output(y:y_end, x:x_end, :) = patch(1:ph, 1:pw, :);
        end
    end
end
