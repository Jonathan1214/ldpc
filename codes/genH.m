%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% @file name:       genH.m
% @created date:    2021/05/12
% @author:          Jiangxuan Li
% @decription:
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear;
clc;
% add lib file path
addpath(genpath('../../libmatlab'));

root = 3;
q    = 257;

% tableIndexOfPower: index means power, while value means elements in GF(q)
% tablePowerOfIndex: index means elements in GF(a), while value means 
%                                the power based on root.
[tablePowerOfIndex, tableIndexOfPower, W1] = genBaseMatOfQCLDPC(root, q);

g = 4;
r = 24;
row_start = q-g;
col_start = 1;
% h = waitbar(0, 'wait');
% tic
% for row_start = 37:253
% cur_prog = (row_start - 36) / 216;
% todisp = sprintf('Cur row_start: %d, Progress: %.2f %%  Time used: %.2f s', row_start, cur_prog*100, toc);
% waitbar(cur_prog, h, todisp);
H_disp = W1(row_start:row_start+g-1, col_start:col_start+r-1);

% convert H_disp to a real parity-check matrix H, using two matrix 
[CNs_connection, VNs_connection] = genConnMat(q, H_disp, tablePowerOfIndex);

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
% dlmwrite('../matrix/VNsCon.csv', VNs_connection)
% dlmwrite('../matrix/CNsCon.csv', CNs_connection)













