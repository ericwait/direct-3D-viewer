function [ output_args ] = open( input_args )
%OPEN Summary of this function goes here
%   Detailed explanation goes here

[arrowFaces, arrowVerts, arrowNorms] = MakeArrow(0.65,0.05,0.15,40);
[sphereFaces, sphereVerts, shereNorms] = MakeSphere(0.20,40);

d3dViewer.d3dViewer('init',arrowFaces, arrowVerts, arrowNorms,sphereFaces, sphereVerts, shereNorms);
end
