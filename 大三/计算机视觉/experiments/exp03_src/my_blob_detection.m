function my_blob_detection(img_path)
    % Parameters
    initial_sigma = 2.0;
    k = 1.5;
    n = 8;
    threshold = 0.03;
    
    % Load image
    img = imread(img_path);
    
    % Convert to grayscale if RGB
    if size(img, 3) == 3
        img_gray = double(rgb2gray(img));
    else
        img_gray = double(img);
    end
    
    [rows, cols] = size(img_gray);
    
    % Build Laplacian scale space
    fprintf('Building Laplacian scale space...\n');
    
    % Store Laplacian responses at each scale
    scale_space = zeros(rows, cols, n);
    scales = zeros(1, n);
    
    for i = 1:n
        sigma = initial_sigma * (k^(i-1));
        scales(i) = sigma;
        
        fprintf('Processing scale %.2f...\n', sigma);
        
        % Generate LoG filter (Laplacian of Gaussian)
        % LoG = (x^2 + y^2 - 2*sigma^2) / (sigma^4) * exp(-(x^2+y^2)/(2*sigma^2))
        filter_size = ceil(6 * sigma);
        if mod(filter_size, 2) == 0
            filter_size = filter_size + 1;
        end
        
        [X, Y] = meshgrid(-filter_size/2:filter_size/2, -filter_size/2:filter_size/2);
        
        % Compute LoG filter (scale-normalized)
        sigma4 = sigma^4;
        sigma2 = 2 * sigma^2;
        X2Y2 = X.^2 + Y.^2;
        log_filter = (X2Y2 - sigma2) / sigma4 .* exp(-X2Y2 / sigma2);
        
        % Normalize filter to make it scale-invariant
        log_filter = log_filter / sum(abs(log_filter(:)));
        
        % Apply LoG filter to image
        log_response = imfilter(img_gray, log_filter, 'replicate', 'conv');
        
        % Store square of Laplacian response (for blob detection)
        scale_space(:, :, i) = log_response.^2;
    end
    
    fprintf('Scale space built successfully.\n');
    
    % Nonmaximum suppression in scale space
    fprintf('Performing nonmaximum suppression...\n');
    
    % Initialize detection map
    detection_map = zeros(rows, cols);
    detected_sigmas = [];
    detected_centers = [];
    
    % Find local maxima in 3D scale space
    for i = 2:n-1
        current_response = scale_space(:, :, i);
        
        % Compare with neighbors in scale space
        for r = 2:rows-1
            for c = 2:cols-1
                value = current_response(r, c);
                
                % Check if it's a local maximum in 3x3x3 neighborhood
                if value > threshold
                    is_max = true;
                    % Check spatial neighbors
                    for dr = -1:1
                        for dc = -1:1
                            if current_response(r+dr, c+dc) > value
                                is_max = false;
                                break;
                            end
                        end
                        if ~is_max, break; end
                    end
                    
                    % Check scale neighbors
                    if is_max
                        if scale_space(r, c, i-1) > value || scale_space(r, c, i+1) > value
                            is_max = false;
                        end
                    end
                    
                    if is_max
                        detection_map(r, c) = value;
                        detected_sigmas = [detected_sigmas, scales(i)];
                        detected_centers = [detected_centers; [c, r]];
                    end
                end
            end
        end
    end
    
    fprintf('Found %d blobs.\n', length(detected_sigmas));
    
    % Display results
    figure;
    imshow(img);
    hold on;
    
    % Draw circles at characteristic scales
    % Radius in image space = sqrt(2) * sigma for LoG
    colors = ['r', 'g', 'b', 'c', 'm', 'y'];
    
    for i = 1:min(length(detected_sigmas), 500)  % Limit to 500 circles for clarity
        sigma = detected_sigmas(i);
        radius = sqrt(2) * sigma;
        center = detected_centers(i, :);
        
        color = colors(mod(i, length(colors)) + 1);
        rectangle('Position', [center(1)-radius, center(2)-radius, 2*radius, 2*radius], ...
                  'Curvature', [1 1], ...
                  'EdgeColor', color, ...
                  'LineWidth', 1.5);
    end

    title(sprintf('Blob Detection (LoG) - %d blobs detected', length(detected_sigmas)));
    hold off;
    saveas(gcf,'blob_detection_result1.png');

    % Also display the scale space response
    figure;
    for i = 1:n
        subplot(2, ceil(n/2), i);
        imshow(scale_space(:, :, i), []);
        title(sprintf('Scale %.2f', scales(i)));
    end
    subtitle('Laplacian Scale Space Responses');
    
    % Save the result
    saveas(gcf, 'blob_detection_scale_space1.png');
    
    fprintf('Results saved.\n');
end