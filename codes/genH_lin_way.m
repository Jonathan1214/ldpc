
clear;
clc;
% add lib file path
addpath(genpath('../../libmatlab'));

root = 3;
q    = 257;
q_m1 = q - 1;

[tablePowerOfIndex, tableIndexOfPower, W1] = genBaseMatOfQCLDPC(root, q);

g = 4;
r = 24;
row_start = q-g;
col_start = 1;

H_disp = W1(row_start:row_start+g-1, col_start:col_start+r-1);

% convert H_disp to a real parity-check matrix H, using two matrix 
[CNs_connection, VNs_connection] = genConnMat(q, H_disp, tablePowerOfIndex);

% test rank of matrix to decide encode ratio of LDPC code
[r_II_ex1, HH] = myrank(CNs_connection, q-1, 0);

%% 

% least 5 columns to obtain rank 1021
k = r - g;
l = 4;
lb = l * (q - 1);

k = 1;
Dd = H_disp(:, k:k+l-1);
[cnc, ~] = genConnMat(q, Dd, tablePowerOfIndex);
[rk, D] = myrank(cnc, q-1, 0);

% switch HH
% lst = HH(:, (r-1)*q_m1+1:r*q_m1);
% HH(:, (r-l)*q_m1+1:r*q_m1) = HH(:, (k-1)*q_m1+1:(k+l-1)*q_m1);
% HH(:, (k-1)*q_m1+1:k*q_m1) = lst;

HH = HH(:, (k+l-1)*q_m1+1:end);

col_sel = [1:256 258:512 514:768 770:1024];
elimi_col = [257 513 769];
newCH = D(:, col_sel);

nr = gfrank(newCH);
%% for G part
% G 部分的计算和 H 满秩时的计算方法相同
DD = D;
DD(:, elimi_col) = zeros(length(g*q_m1));
G_g = zeros(r-l, l*q_m1);
u = zeros(q_m1, 1);
u(1) = 1;
z_i = zeros(g * q_m1, r-l);
check = zeros(r-l, 1);
for i = 1:r-l
    M_i = HH(:, (i-1)*q_m1+1:i*q_m1);
    z_i(:, i) = mod(M_i * u, 2);
end

[Rr, pivv] = myrref([DD z_i]);
gi = zeros(1, l*q_m1);

for i = 1:r-l
    col = i + lb;
    for j = 1:length(pivv)
        gi(pivv(j)) = Rr(j, col);
    end
    check(i) = sum(z_i(:, i) ~= mod(D * gi', 2));
    G_g(i, :) = gi;
end


% save one part for hardware test
fid = fopen('../matrix/part_G.txt', 'w');
for row = 1:r-l
    data = sprintf('%d', G_g(row, 1:256));
    fprintf(fid, [data '\n']);
end
fclose(fid);

%% for Q part
% 这一段是固定写法，换成别的 H 非满秩的 LDPC 码也可以如此计算
[R, piv] = myrref(D);

indep_col = [];
for i = 2:rk
    % 找到不存在的列
    if piv(i) ~= piv(i-1) + 1
        indep_col = [indep_col piv(i)-1];
    end
end

if length(indep_col) < lb - rk
    indep_col = [indep_col length(D)];
end

w = zeros(length(indep_col), lb);
for i = 1:length(indep_col)
    idx = indep_col(i);
    wi = zeros(1, l*q_m1);
    for j = 1:length(piv)
        wi(piv(j)) = R(j, idx);
    end
    wi(idx) = 1;
    w(i, :) = wi;
end

r = 12;
Q = [w zeros(lb - rk, (r-l)*q_m1)];

%% full generate matrix
Part_G = zeros((r-l)*q_m1, l*q_m1);

for i = 1:r-l
    Part_G((i-1)*q_m1+1, :) = G_g(i, :);
end

for i = 1:r-l
    for j = 2:q_m1
        for n = 1:l
            Part_G((i-1)*q_m1+j, (n-1)*q_m1+1:n*q_m1) = circshift(Part_G((i-1)*q_m1+j-1, (n-1)*q_m1+1:n*q_m1), 1);
        end
    end
end

% part G encode message example
% message_t = ones(1, q_m1);
% message_t(1) = 0;
% messages = repmat(message_t, 1, r-l);
% out = mod(messages * Part_G, 2);

% 

I_G = eye((r-l)*q_m1);
G = [Part_G I_G];

G_full = [G;Q];

sum(sum(mod([D HH(:, 1:(r-l)*q_m1)] * G_full', 2)))




