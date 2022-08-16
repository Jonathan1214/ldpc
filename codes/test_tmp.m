%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% 
% date: 2022/08/08 23:26
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

basefilename = '../matrix/';
allone = ones(1, 1024);
blkout = ones(5, 1024);
for i = 1:5
    s = (i - 1) * 4;
    e = i * 4;
    part_g = Part_G(s*q_m1+1:e*q_m1, :);
    % 注意这里必须要取转置
    part_g = part_g';
    pfilename = [basefilename 'partGFile' num2str(i) '.csv'];
    fp = fopen(pfilename, 'w');
    dlmwrite(pfilename, part_g, ',');
    fclose(fp);
    disp(pfilename);
    
    part_g = part_g';
    out1 = mod(allone * part_g, 2);
    blkout(i, :) = mod(out1, 2);
    outlog = dlmread(['../test-output-logs/blk' num2str(i) '_test.txt'], ' ');
    outlog = outlog(1:1024);
    sum(outlog ~= blkout(i, :))
end


