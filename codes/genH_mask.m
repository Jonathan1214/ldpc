%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% @file name:       genH_masked.m
% @created date:    2022/02/02
% @author:          Jiangxuan Li
% @decription:
%                   base 4x24
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear;
clc;
% add lib file path
addpath(genpath('../../libmatlab'));

root = 3;
q    = 257;

% tablePowerOfIndex: index means power, while value means elements in GF(q)
% tableIndexOfPower: index means elements in GF(a), while value means 
%                                the power based on root.
[tablePowerOfIndex, tableIndexOfPower, W1] = genBaseMatOfQCLDPC(root, q);

g = 4;
r = 24;
row_start = q-g;
col_start = 1;

% add masking
z_1 = [1 0 1 0;
       0 1 0 1;
       1 1 1 1;
       1 1 1 1];
z_2 = circshift(z_1, [2 0]);
z_mask = repmat([z_1, z_2], 1, floor(r / 8));
if (mod(r, 8) ~= 0)
    z_mask = [z_mask z_1];
end

H_disp = W1(row_start:row_start+g-1, col_start:col_start+r-1);

H_disp_masked = H_disp .* z_mask;

% convert H_disp to a real parity-check matrix H, using two matrix 
[CNs_connection, VNs_connection] = genConnMat(q, H_disp_masked, tablePowerOfIndex);

% test rank of matrix to decide encode ratio of LDPC code
% r_II_ex1 = myrank(CNs_connection, q-1, 0);

% if r_II_ex1 == 1024
%     fprintf('found!!!! row_start %d\n', row_start);
% end
% end
% toc
% draw H
drawParityCheckpMatrix(VNs_connection, q-1, 0);

% save H

% [~, msg] = mkdir('../matrix');
% dlmwrite('../matrix/VNsCon_mask.csv', VNs_connection)
% dlmwrite('../matrix/CNsCon_mask.csv', CNs_connection)


