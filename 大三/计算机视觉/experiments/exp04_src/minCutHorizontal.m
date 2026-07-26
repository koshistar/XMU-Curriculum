function mask = minCutHorizontal(error_surface)
    %% 水平方向最小割缝
    %  error_surface : 误差面 (H, W)，H 为重叠高度
    %  mask          : (H, W) 逻辑矩阵，True=取旧 patch

    % 转置后调用竖直方向，再转置回来
    mask = minCutVertical(error_surface.')';
end
