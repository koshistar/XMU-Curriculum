function my_harriscornerdetection(img_path,k,threshold,sigma)
    img=imread(img_path);
    
    if size(img,3)==3
        image=rgb2gray(img);
    end
    image=double(image);

    %计算梯度
    sobel_x=[-1,0,1;-2,0,2;-1,0,1];
    sobel_y=[1,2,1;0,0,0;-1,-2,-1];
    Ix=imfilter(image,sobel_x,'same','conv');
    Iy=imfilter(image,sobel_y,'same','conv');

    %计算梯度乘积
    Ix2=Ix.^2;
    Iy2=Iy.^2;
    Ixy=Ix.*Iy;

    window_size=2*ceil(3*sigma)+1;
    guassian_filter=fspecial('gaussian',window_size,sigma);
    Sx2=imfilter(Ix2,guassian_filter);
    Sy2=imfilter(Iy2,guassian_filter);
    Sxy=imfilter(Ixy,guassian_filter);

    % 计算响应R
    [rows,cols]=size(image);
    R=zeros(rows,cols);
    for i=1:rows
        for j=1:cols
            H=[Sx2(i,j),Sxy(i,j);Sxy(i,j),Sy2(i,j)];
            R(i,j)=det(H)-k*(trace(H))^2;
        end
    end

    R_normalized=mat2gray(R);
    imwrite(R_normalized,strcat(img_path,'_harris_response_k',num2str(k),'_threshold',num2str(threshold),'_sigma',num2str(sigma),'.png'));

    corners=zeros(size(R));
    Rmax=max(R(:));
    for i=2:rows-1
        for j=2:cols-1
            if R(i,j)>threshold*Rmax && R(i,j)==max(max(R(i-1:i+1,j-1:j+1)))
                corners(i,j)=1;
            end
        end
    end
    % 用内置函数实现
    % Rmax=ordfilt2(R,9,ones(3,3));
    % Rnums=(R==Rmax);
    % R(Rnums==0)=0;
    % Rmax=max(R(:));
    % corners=R>(threshold*Rmax);
    
    [y,x]=find(corners);
    corners=[x,y];

    image_marked=im2double(img);
    for i=1:size(corners,1)
        x=corners(i,1);
        y=corners(i,2);
        if x>1 && x<cols && y>1 && y<rows
            image_marked(y-1:y+1, x-1:x+1, 1) = 1.0;
            image_marked(y-1:y+1, x-1:x+1, 2) = 0;
            image_marked(y-1:y+1, x-1:x+1, 3) = 0;
        end
    end

    imwrite(image_marked,strcat(img_path,'_harris_corners_k',num2str(k),'_threshold',num2str(threshold),'_sigma',num2str(sigma),'.png'));
    figure("Name","Harris Corner Detection");
    subplot(1,4,1);
    imshow(img);
    title("Input Image");
    subplot(1,4,2);
    imshow(R_normalized);
    title("Harris Response");
    subplot(1,4,3);
    imshow(image_marked);
    title("Harris Corners");
    subplot(1,4,4);
    plot(corners(:,1),corners(:,2),'r*');
end