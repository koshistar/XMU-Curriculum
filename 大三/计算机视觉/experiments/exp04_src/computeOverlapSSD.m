function errors = computeOverlapSSD(texture, patch_size, overlap, output, y, x, out_H, out_W)
    %% 计算候选 patch 与重叠区域的 SSD 误差矩阵
    %  texture    : 纹理样本图 (H, W, 3)
    %  patch_size : patch 边长
    %  overlap    : 重叠宽度
    %  output     : 当前输出画布
    %  y, x       : 当前待填位置的左上角坐标
    %  out_H, out_W : 输出画布尺寸
    %  errors     : 误差矩阵 (n_y, n_x)

    [H, W, ~] = size(texture);
    n_y = H - patch_size + 1;
    n_x = W - patch_size + 1;
    errors = zeros(n_y, n_x);

    % 左侧重叠区域
    if x > 1
        y2 = min(y + patch_size - 1, out_H);
        x2 = min(x + overlap - 1, out_W);
        ref = double(output(y:y2, x:x2, :));
        [rh, rw, ~] = size(ref);
        if rh == patch_size && rw == overlap
            for ty = 1:n_y
                for tx = 1:n_x
                    cand = double(texture(ty:ty+patch_size-1, tx:tx+overlap-1, :));
                    diff = cand - ref;
                    errors(ty, tx) = errors(ty, tx) + sum(sum(sum(diff .^ 2)));
                end
            end
        end
    end

    % 上方重叠区域
    if y > 1
        y2 = min(y + overlap - 1, out_H);
        x2 = min(x + patch_size - 1, out_W);
        ref = double(output(y:y2, x:x2, :));
        [rh, rw, ~] = size(ref);
        if rh == overlap && rw == patch_size
            for ty = 1:n_y
                for tx = 1:n_x
                    cand = double(texture(ty:ty+overlap-1, tx:tx+patch_size-1, :));
                    diff = cand - ref;
                    errors(ty, tx) = errors(ty, tx) + sum(sum(sum(diff .^ 2)));
                end
            end
        end
    end

end
