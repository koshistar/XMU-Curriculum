function [best_ty, best_tx] = choosePatch(errors, tolerance)
    %% 在 SSD 容忍度范围内随机选择最优 patch
    %  errors    : SSD 误差矩阵 (n_y, n_x)
    %  tolerance  : 容忍度（误差 ≤ (1+tolerance)*min_error 均为候选）
    %  best_ty, best_tx : 选中的 patch 在纹理图中的左上角坐标

    min_err = min(errors(:));
    candidates = find(errors <= min_err * (1 + tolerance));

    if isempty(candidates)
        % 极端情况：所有误差都很大，选最小的一个
        [~, idx] = min(errors(:));
        candidates = idx;
    end

    % 随机选一个候选
    idx = candidates(randi(length(candidates)));
    [best_ty, best_tx] = ind2sub(size(errors), idx);
end
