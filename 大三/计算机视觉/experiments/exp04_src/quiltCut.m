function output = quiltCut(texture, out_H, out_W, patch_size, overlap, tolerance)
    %% 最优 patch 选择 + 最小割缝合
    %  texture    : 纹理样本图 (H, W, 3) double [0,1]
    %  out_H, out_W : 输出图像高、宽
    %  patch_size : patch 边长
    %  overlap   : 重叠宽度
    %  tolerance  : SSD 容忍度
    %  output     : 合成结果 (out_H, out_W, 3)

    [H, W, C] = size(texture);
    output = zeros(out_H, out_W, C);
    step = patch_size - overlap;

    rows = 1:step:out_H;
    cols = 1:step:out_W;

    for yi = 1:length(rows)
        y = rows(yi);
        for xi = 1:length(cols)
            x = cols(xi);

            y_end = min(y + patch_size - 1, out_H);
            x_end = min(x + patch_size - 1, out_W);
            ph = y_end - y + 1;
            pw = x_end - x + 1;

            %% 选择最优 patch
            if yi == 1 && xi == 1
                % 第一个 patch：随机选
                ty = randi(H - patch_size + 1);
                tx = randi(W - patch_size + 1);
            else
                % 计算重叠区域 SSD
                errors = computeOverlapSSD(texture, patch_size, overlap, output, y, x, out_H, out_W);
                [ty, tx] = choosePatch(errors, tolerance);
            end

            ty_end=min(ty + patch_size - 1, H);
            tx_end=min(tx + patch_size - 1, W);
            new_patch = double(texture(ty:ty_end, tx:tx_end, :));

            %% 最小割缝融合
            blended = new_patch;

            % 左侧重叠：竖直方向割缝
            if x > 1
                overlap_w=min(overlap, pw);
                old_left = output(y:y_end, x:x+overlap_w-1, :);
                new_left = blended(:, 1:overlap, :);
                if size(old_left,1)==size(new_left,1)&&size(old_left,2)==size(new_left,2)&&~isempty(old_left)
                    err_surf = sum((old_left - new_left).^2, 3);
                    mask = minCutVertical(err_surf);
                    for c = 1:C
                        blended(:, 1:overlap, c) = ( mask  .* old_left(:,:,c)) + (~mask .* new_left(:,:,c));
                    end
                end
            end

            % 上方重叠：水平方向割缝
            if y > 1
                overlap_h=min(overlap, ph);
                old_top = output(y:y+overlap_h-1, x:x_end, :);
                new_top = blended(1:overlap_h, :, :);
                if size(old_top,1)==size(new_top,1)&&size(old_top,2)==size(new_top,2)&&~isempty(old_top)
                    err_surf = sum((old_top - new_top).^2, 3);
                    mask = minCutHorizontal(err_surf);
                    for c = 1:C
                        blended(1:overlap, :, c) = ( mask  .* old_top(:,:,c)) + (~mask .* new_top(:,:,c));
                    end
                end
            end

            % 写入输出
            output(y:y_end, x:x_end, :) = blended(1:ph, 1:pw, :);
        end
    end
end
