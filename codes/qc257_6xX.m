%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% @file name:       qc257_6xX.m
% @created date:    2022/07/13
% @author:          Jiangxuan Li
% @decription:
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear;
clc;
% add lib file path
addpath(genpath('../ldpc-constructor-lib'));

% The best solution is to get a number
% GF file setup
root = 3;
q    = 257;
% row of base matrix
g = 6;
% col of base matrix
r = input('input COLUMN of base matrix: ');
if size(r) == 0
    disp('use default column of base matrix, which equals 20\n');
    r = 20;
end
% start row of base matrix
row_start = q-g;
% start col of base matrix
col_start = 1;

code_len = r * (q - 1);
rowH = g * (q - 1);
% tableIndexOfPower: index means power, while value means elements in GF(q)
% tablePowerOfIndex: index means elements in GF(a), while value means 
%                                the power based on root.
[tablePowerOfIndex, tableIndexOfPower, W1] = genBaseMatOfQCLDPC(root, q);

H_disp = W1(row_start:row_start+g-1, col_start:col_start+r-1);

% convert H_disp to a real parity-check matrix H, using two matrix 
[CNs_connection, VNs_connection] = genConnMat(q, H_disp, tablePowerOfIndex);

% test rank of matrix to decide encode ratio of LDPC code
disp('H matrix steup done.');
r_II_ex1 = myrank(CNs_connection, q-1, 0);
info_len = code_len - r_II_ex1;
fprintf('code size (code_len, info_len): (%d, %d)\n', code_len, info_len);

% draw H
drawParityCheckpMatrix(VNs_connection, q-1, 0);

% save H matrix
file_path     = '..';
sub_fold      = '/matrix/';
vnfile_prefix = 'VNsCon'    ;
cnfile_prefix = 'CNsCon'    ;
file_suffix   = '.csv'      ;
delim         = ','         ;
bmsize        = [num2str(g) 'x' num2str(r)];
vnfile_sub    = [sub_fold vnfile_prefix num2str(q) '_' bmsize file_suffix];
cnfile_sub    = [sub_fold cnfile_prefix num2str(q) '_' bmsize file_suffix];
vnfile        = [file_path vnfile_sub];
cnfile        = [file_path cnfile_sub];

dlmwrite(vnfile, VNs_connection, delim);
dlmwrite(cnfile, CNs_connection, delim);

% filepath of Generator matrix
G_file_sub = [sub_fold 'f' file_suffix];

% makefile configuration
mkconf_path = [file_path '/code-conf/'];
mkconf = fopen([mkconf_path 'makefile.' num2str(q) '_' bmsize], 'w');
fprintf(mkconf, ['run' bmsize ':\n']);
fprintf(mkconf, '\t@$(TARGET) \\\n');
fprintf(mkconf, ['\t\t-l ' num2str(code_len) ' \\\n']);
fprintf(mkconf, ['\t\t-r ' num2str(rowH)     ' \\\n']);
fprintf(mkconf, ['\t\t-b ' num2str(info_len) ' \\\n']);
fprintf(mkconf, ['\t\t-d0 '                  ' \\\n']);
fprintf(mkconf, ['\t\t-i50'                  ' \\\n']);
fprintf(mkconf, ['\t\t-a625'                 ' \\\n']);
fprintf(mkconf, ['\t\t-C ' num2str(r)        ' \\\n']);
fprintf(mkconf, ['\t\t-V ' num2str(g)        ' \\\n']);
fprintf(mkconf, ['\t\t-G ' num2str(q-1)      ' \\\n']);
fprintf(mkconf, ['\t\t-x$(CUR_DIR)' G_file_sub ' \\\n']);
fprintf(mkconf, ['\t\t-c$(CUR_DIR)' cnfile_sub ' \\\n']);
fprintf(mkconf, ['\t\t-v$(CUR_DIR)' vnfile_sub ' \\\n']);
fprintf(mkconf, ['\t\t-s 2.5'                  ' \\\n']);
fprintf(mkconf, ['\t\t-S 0.2'                  ' \\\n']);
fprintf(mkconf, ['\t\t-E 3.5'                  ' \\\n']);
fprintf(mkconf, '\t\t$(outfile)\n');
fclose(mkconf);

disp('ALL JOB DONE GOOD');

