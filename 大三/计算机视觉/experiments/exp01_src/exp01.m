function exp01(image_path)
    srcImg=imread(image_path);
    if size(srcImg,3)==3
        grayImg=rgb2gray(srcImg);
    else
        grayImg=srcImg;
    end
    grayImg=im2double(grayImg);

    figure('Name','Original Picture','Color','w');
    imshow(grayImg);
    title('Original Image');

    % Add noise
    %noise_mean=0;
    %noise_var=0.01;
    %noisyImg=imnoise(grayImg,'gaussian',noise_mean,noise_var);
    noisyImg=grayImg;

    % Apply Traditional Gaussian Filter
    gauss_sigma_1=2;
    gauss_filtered_1=imgaussfilt(noisyImg,gauss_sigma_1);
    gauss_sigma_2=6;
    gauss_filtered_2=imgaussfilt(noisyImg,gauss_sigma_2);
    gauss_sigma_3=18;
    gauss_filtered_3=imgaussfilt(noisyImg,gauss_sigma_3);

    % Apply Bilateral Filter
    sigma_d_1=2;
    sigma_r_A=0.1; 
    result_A_1=my_bilateral_filter(noisyImg,sigma_d_1,sigma_r_A);
    sigma_r_B=0.25;
    result_B_1=my_bilateral_filter(noisyImg,sigma_d_1,sigma_r_B);
    sigma_r_C=1;
    result_C_1=my_bilateral_filter(noisyImg,sigma_d_1,sigma_r_C); 
    sigma_d_2=6;
    result_A_2=my_bilateral_filter(noisyImg,sigma_d_2,sigma_r_A);
    result_B_2=my_bilateral_filter(noisyImg,sigma_d_2,sigma_r_B);
    result_C_2=my_bilateral_filter(noisyImg,sigma_d_2,sigma_r_C); 
    sigma_d_3=18; 
    result_A_3=my_bilateral_filter(noisyImg,sigma_d_3,sigma_r_A);
    result_B_3=my_bilateral_filter(noisyImg,sigma_d_3,sigma_r_B);
    result_C_3=my_bilateral_filter(noisyImg,sigma_d_3,sigma_r_C); 

    figure('Name', 'Bilateral Filtering Analysis', 'Color', 'w');
    subplot(3,4,1)
    imshow(gauss_filtered_1);
    title(['Gaussian Filter (\sigma_d=',num2str(gauss_sigma_1),')']);
    subplot(3,4,2)
    imshow(result_A_1);
    title(['Bilateral A (\sigma_d=',num2str(sigma_d_1),', \sigma_r=',num2str(sigma_r_A),')']);
    subplot(3,4,3)
    imshow(result_B_1);
    title(['Bilateral B (\sigma_d=',num2str(sigma_d_1),', \sigma_r=',num2str(sigma_r_B),')']);
    subplot(3,4,4)
    imshow(result_C_1);
    title(['Bilateral C (\sigma_d=',num2str(sigma_d_1),', \sigma_r=',num2str(sigma_r_C),')']);
    subplot(3,4,5)
    imshow(gauss_filtered_2);
    title(['Gaussian Filter (\sigma_d=',num2str(gauss_sigma_2),')']);
    subplot(3,4,6)
    imshow(result_A_2);
    title(['Bilateral A (\sigma_d=',num2str(sigma_d_2),', \sigma_r=',num2str(sigma_r_A),')']);
    subplot(3,4,7)
    imshow(result_B_2);
    title(['Bilateral B (\sigma_d=',num2str(sigma_d_2),', \sigma_r=',num2str(sigma_r_B),')']);
    subplot(3,4,8)
    imshow(result_C_2);
    title(['Bilateral C (\sigma_d=',num2str(sigma_d_2),', \sigma_r=',num2str(sigma_r_C),')']);
    subplot(3,4,9)
    imshow(gauss_filtered_3);
    title(['Gaussian Filter (\sigma_d=',num2str(gauss_sigma_3),')']);
    subplot(3,4,10)
    imshow(result_A_3);
    title(['Bilateral A (\sigma_d=',num2str(sigma_d_3),', \sigma_r=',num2str(sigma_r_A),')']);
    subplot(3,4,11)
    imshow(result_B_3);
    title(['Bilateral B (\sigma_d=',num2str(sigma_d_3),', \sigma_r=',num2str(sigma_r_B),')']);
    subplot(3,4,12)
    imshow(result_C_3);
    title(['Bilateral C (\sigma_d=',num2str(sigma_d_3),', \sigma_r=',num2str(sigma_r_C),')']);

    imwrite(gauss_filtered_1,'gauss_filtered_1.png');
    imwrite(gauss_filtered_2,'gauss_filtered_2.png');
    imwrite(gauss_filtered_3,'gauss_filtered_3.png');
    imwrite(result_A_1,'bilateral_filtered_A_1.png');
    imwrite(result_B_1,'bilateral_filtered_B_1.png');
    imwrite(result_C_1,'bilateral_filtered_C_1.png');
    imwrite(result_A_2,'bilateral_filtered_A_2.png');
    imwrite(result_B_2,'bilateral_filtered_B_2.png');
    imwrite(result_C_2,'bilateral_filtered_C_2.png');
    imwrite(result_A_3,'bilateral_filtered_A_3.png');
    imwrite(result_B_3,'bilateral_filtered_B_3.png');
    imwrite(result_C_3,'bilateral_filtered_C_3.png');
end
