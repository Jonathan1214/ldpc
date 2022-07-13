clear; clc;

m = 1024;
n = 6144;
r = 24;
H_full = zeros(m, n);

CNs_connection = dlmread('../matrix/CNsCon.csv');
for i = 1:m
    for j = 1:r
        if CNs_connection(i, j) ~= 0
            H_full(i, CNs_connection(i, j)) = 1;
        end
    end
end

% 求出生成矩阵

[m,n] = size(H_full);
[R,pivcol] = myrref(H_full);
% R = mod(R, 2);
r = length(pivcol); 
nopiv = 1:n;
nopiv(pivcol) = [];
G = zeros(n,n-r,class(H_full));
if n > r
  G(nopiv,:) = eye(n-r,n-r,class(H_full));
  if r > 0
     G(pivcol,:) = -R(1:r,nopiv);
  end
end
GG = mod(G', 2);
imagesc(mod(G, 2));

% save('GG.mat', 'GG');
% dlmwrite('../matrix/G_mat.csv', GG, ' ');
% dlmwrite('../matrix/G_sys_posi.csv', nopiv-1, ' ');