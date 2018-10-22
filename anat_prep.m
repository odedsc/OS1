clear all;
close all;
clc;

%% preperation:
%getting image pixels by loop:
im=zeros(127,127);
for x=1:127
    for y=1:127
        im(y,x)=(128-x+y);
    end;
end;
imshow(im,[]);

%now getting the same with meshgrid:
[X Y]=meshgrid(1:1:127);
im=(128-X+Y);
imshow(im,[]);

figure(1);imshow(im,[0 127]); impixelinfo;
figure(2);imshow(im,[50 150]);impixelinfo;
figure(3);imshow(im,[150 255]); impixelinfo;
figure(4);imshow(im,[]); impixelinfo;

%% lab2:
level=200;
window=50;
new_im=im;
for x=1:127
    for y=1:127
        if(im(y,x)>(level+window/2))
            new_im(y,x)=255;
        elseif  (im(y,x)<(level-window/2))
            new_im(y,x)=0;
        else
            new_im(y,x)=(im(y,x)-(level-window/2))/window*255;
        end;
    end;
end;

figure(5);
plot(im,new_im);
title('old image pixels as function of new after converting');
xlabel('old image pixels');
ylabel('new image pixels after converting');

figure(6);imshow(new_im,[]); impixelinfo;

%% lab3:

BW = ([1 1 0 ; 1 1 0 ; 0 0 1]);
CC1 = bwconncomp(BW, 4);
CC2 = bwconncomp(BW, 8);
figure(7);imshow(BW); 

BW = imread('text.png');

new_im2=im;
for x=1:127
    for y=1:127
        if(im(y,x)>50)
            new_im2(y,x)=255;
        elseif  (im(y,x)<=50)
            new_im2(y,x)=0;
        end;
    end;
end;

figure(8);
plot(im,new_im2);
title('old image pixels as function of new after converting');
xlabel('old image pixels');
ylabel('new image pixels after converting');

figure(9); histogram(im);
figure(10); histogram(new_im2);

%% lab4:
I = imread('pout.tif');
h = 1/5*ones(5,1);
H = h*h';
imfilt = filter2(H,I);
figure(11);imshowpair(I,imfilt,'montage')