function Processing() 
basename = 'newData1';
[status,result] = dos(strcat('C:\AVL\avl326.exe < ',basename,'.run'));
% disp('Running AVL...');
% disp(result);
