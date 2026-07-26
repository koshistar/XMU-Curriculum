function mask = minCutVertical(error_surface)
    %% 竖直方向动态规划找最小代价路径
    %  error_surface : 误差面 (H, W)，W 为重叠宽度
    %  mask          : (H, W) 逻辑矩阵，True=取旧 patch，False=取新 patch
    %  路径左侧（x < seam_x）为 True，右侧为 False

    [H, W] = size(error_surface);
    dp = double(error_surface);
    prev=zeros(H, W,'int32');

    % 动态规划：从第二行开始累加最小代价
    for i = 2:H
        for j = 1:W
            left=max(1,j-1);
            right=min(W,j+1);
            [min_val, idx] = min(dp(i-1, left:right));
            dp(i, j) = dp(i, j) + min_val;
            prev(i, j) = idx + left - 1;
        end
    end

    % 回溯最优路径：mask(i, j)=true 表示路径左侧
    mask = true(H, W);
    [~, j] = min(dp(H, :));
    j=max(1,min(W,j));

    for i = H:-1:1
        mask(i, j+1:end) = false;
        if i > 1
            j=prev(i, j);
        end
    end

end
