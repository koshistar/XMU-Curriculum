function outputImg=my_bilateral_filter(inputImg,sigma_d,sigma_r)
    img=im2double(inputImg);
    [rows,cols]=size(img);

    kernel_radius=ceil(3*sigma_d);
    [x,y]=meshgrid(-kernel_radius:kernel_radius,-kernel_radius:kernel_radius);
    G_spatial=exp(-(x.^2+y.^2)/(2*sigma_d^2));

    outputImg=zeros(rows,cols);
    paddedImg=padarray(img,[kernel_radius,kernel_radius],'symmetric');
    for i=1:rows
        for j=1:cols
            r_start=i;
            r_end=i+2*kernel_radius;
            c_start=j;
            c_end=j+2*kernel_radius;

            local_window=paddedImg(r_start:r_end,c_start:c_end);
            center_val=paddedImg(i+kernel_radius,j+kernel_radius);
            diff=local_window-center_val;
            G_range=exp(-(diff.^2)/(2*sigma_r^2));
            W=G_spatial.*G_range;
            W_norm=W/sum(W(:));
            outputImg(i,j)=sum(sum(W_norm.*local_window));
        end
    end
end