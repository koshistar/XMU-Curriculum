function output = textureTransfer(texture, target, patch_size, overlap, alpha, tolerance, prev_output)
    %% 纹理迁移
    %  texture   : 纹理样本图 (H, W, 3) double [0,1]
    %  target    : 目标图像 (out_H, out_W, 3) 与输出同尺寸
    %  patch_size: patch 边长
    %  overlap   : 重叠宽度
    %  alpha     : 外观约束权重（alpha越大越忠实于目标图）
    %  tolerance : SSD 容忍度
    %  output    : 迁移结果 (out_H, out_W, 3)

    if nargin<7||isempty(prev_output)
        prev_output=[];
    end

    [H, W, C] = size(texture);
    [out_H, out_W, ~] = size(target);
    output = zeros(out_H, out_W, C);
    step = patch_size - overlap;

    % 目标图与纹理样本转灰度（用于外观约束）
    if size(target, 3) >= 3; target_gray = im2double(rgb2gray(target)); else; target_gray = im2double(target(:,:,1)); end
    if size(texture, 3) >= 3; tex_gray = im2double(rgb2gray(texture)); else; tex_gray = im2double(texture(:,:,1)); end

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
            
            %% 计算重叠误差
            if yi == 1 && xi == 1
                overlap_errors = zeros(H - patch_size + 1, W - patch_size + 1);
            else
                overlap_errors = computeOverlapSSD(texture, patch_size, overlap, output, y, x, out_H, out_W);
                % 改进的 Min-Max 归一化
                min_oe = min(overlap_errors(:)); max_oe = max(overlap_errors(:));
                if max_oe > min_oe
                    overlap_errors = (overlap_errors - min_oe) / (max_oe - min_oe);
                else
                    overlap_errors = zeros(size(overlap_errors));
                end
            end
            
            %% 计算外观误差
            tgt_block = target_gray(y:y_end, x:x_end);
            n_y = H - patch_size + 1;
            n_x = W - patch_size + 1;
            appearance_errors = zeros(n_y, n_x);
            
            % 动态裁剪纹理，保证边缘patch也能计算外观误差
            for pty = 1:n_y
                for ptx = 1:n_x
                    cand = tex_gray(pty:pty+ph-1, ptx:ptx+pw-1);
                    appearance_errors(pty, ptx) = sum((cand(:) - tgt_block(:)).^2);
                end
            end
            
            % Min-Max 归一化
            min_ae = min(appearance_errors(:)); 
            max_ae = max(appearance_errors(:));
            if max_ae > min_ae
                appearance_errors = (appearance_errors - min_ae) / (max_ae - min_ae);
            else
                appearance_errors = zeros(size(appearance_errors));
            end
            
            %% 综合误差选择 patch
            total_errors = alpha * appearance_errors + (1 - alpha) * overlap_errors;
            [sel_y, sel_x] = choosePatch(total_errors, tolerance);
            
            ty_end = min(sel_y + patch_size - 1, H);
            tx_end = min(sel_x + patch_size - 1, W);
            new_patch = double(texture(sel_y:ty_end, sel_x:tx_end, :));
            
            %% 最小割缝融合
            blended = new_patch;
            
            % 左侧重叠：竖直割缝
            if x > 1
                ol_w = min(overlap, pw);
                old_left = output(y:y_end, x:x+ol_w-1, :);
                new_left = blended(:, 1:ol_w, :);
                if size(old_left,1)==size(new_left,1) && size(old_left,2)==size(new_left,2)
                    err_surf = sum((old_left - new_left).^2, 3);
                    mask = minCutVertical(err_surf);
                    for c = 1:C
                        blended(:, 1:ol_w, c) = (mask .* old_left(:,:,c)) + (~mask .* new_left(:,:,c));
                    end
                end
            end
            
            % 上方重叠：水平割缝
            if y > 1
                ol_h = min(overlap, ph);
                old_top = output(y:y+ol_h-1, x:x_end, :);
                new_top = blended(1:ol_h, :, :);
                if size(old_top,1)==size(new_top,1) && size(old_top,2)==size(new_top,2)
                    err_surf = sum((old_top - new_top).^2, 3);
                    mask = minCutHorizontal(err_surf);
                    for c = 1:C
                        blended(1:ol_h, :, c) = (mask .* old_top(:,:,c)) + (~mask .* new_top(:,:,c));
                    end
                end
            end
            
            % 写入输出
            output(y:y_end, x:x_end, :) = blended(1:ph, 1:pw, :);
        end
    end
end
