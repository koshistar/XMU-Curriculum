import cv2
import numpy as np
img1=cv2.imread("neko1.jpg")
img2=cv2.imread("neko2.jpg")
img3=cv2.imread("neko3.jpg")
img4=cv2.imread("neko4.jpg")
# cv2.imshow("neko1",img1)
# cv2.imshow("neko2",img2)
# cv2.imshow("neko3",img3)
# cv2.imshow("neko4",img4)
# 等比例缩放
size=420
aspect_ratio=[img1.shape[1]/img1.shape[0],img2.shape[1]/img2.shape[0],img3.shape[1]/img3.shape[0],img4.shape[1]/img4.shape[0]]
img1=cv2.resize(img1,(int(aspect_ratio[0]*size),int(size)),interpolation=cv2.INTER_AREA)
img2=cv2.resize(img2,(int(aspect_ratio[1]*size),int(size)),interpolation=cv2.INTER_AREA)
img3=cv2.resize(img3,(int(aspect_ratio[2]*size),int(size)),interpolation=cv2.INTER_AREA)
img4=cv2.resize(img4,(int(aspect_ratio[3]*size),int(size)),interpolation=cv2.INTER_AREA)
images=[img1,img2,img3,img4]
rows,cols=(2,2)
heights,widths=zip(*[(img.shape[0],img.shape[1]) for img in images])
max_height=max(heights)
max_width=max(widths)
collage=np.zeros((max_height*rows,max_width*cols,3),dtype=np.uint8)
for i,img in enumerate(images):
    row=i//cols
    col=i%cols
    #调整图像大小
    # img_resized=cv2.resize(img,(max_width,max_height))
    # collage[row*max_height:(row+1)*max_height,col*max_width:(col+1)*max_width]=img_resized
    # 调整画布大小
    collage[row*max_height:(row*max_height+img.shape[0]),col*max_width:(col*max_width+img.shape[1])]=img
cv2.imshow('neko',collage)
cv2.imwrite('neko.jpg',collage)
cv2.waitKey(0)
cv2.destroyAllWindows()