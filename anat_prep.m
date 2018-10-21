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
        end;
    end;
end;

figure(5);
plot(im,new_im);
title('old image pixels as function of new after converting');
xlabel('old image pixels');
ylabel('new image pixels after converting');

figure(6);imshow(new_im,[]); impixelinfo;
