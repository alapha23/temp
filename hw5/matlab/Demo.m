% This code implemented a comparison between k-means and mean-shift
% We simplified original code written by Alireza Asvadi

%% clear command windows
clc
clear all
close all
%% input
I    = imread('1.jpg');    % Original: also test 2.jpg
%% parameters
% kmeans parameter
K    = 8;                  % Cluster Numbers
% meanshift parameter
bw   = 0.2;                % Mean Shift Bandwidth

%% compare
Ikm          = Km(I,K);                     % Kmeans (color)
[Ims, Nms]   = Ms(I,bw);                    % Mean Shift (color)
%% show
figure()
subplot(131); imshow(I);    title('Original'); 
subplot(132); imshow(Ikm);  title(['Kmeans',' : ',num2str(K)]);
subplot(133); imshow(Ims);  title(['MeanShift',' : ',num2str(Nms)]);

