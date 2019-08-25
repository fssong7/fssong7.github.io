function preProcessing(alpha,elevator)
%% INPUT variables
filename = 'allegro';
velocity = 10;%m/s
%Define a base file name to save 
basename = 'newData1';

%% Directory Preparation
%Purge Directory of interfering files
[status,result] =dos(strcat('del ',basename,'.st'));
[status,result] =dos(strcat('del ',basename,'.sb'));
[status,result] =dos(strcat('del ',basename,'.run'));
[status,result] =dos(strcat('del ',basename,'.eig'));

%% Create run file
%Open the file with write permission
fid = fopen(strcat(basename,'.run'), 'wt');

%Load the AVL definition of the aircraft
fprintf(fid, 'LOAD %s \n', strcat(filename,'.avl'));

%Load mass parameters
fprintf(fid, 'MASS %s\n', strcat(filename,'.mass'));
fprintf(fid, 'MSET\n');
%Change this parameter to set which run cases to apply 
fprintf(fid, '%i\n',   0); 

%Disable Graphics
fprintf(fid, 'PLOP\ng\n\n'); 

%Open the OPER menu
fprintf(fid, '%s\n',   'OPER');   

%Define the run case
% fprintf(fid, 'c1\n',   'c1');       
% fprintf(fid, 'v %6.4f\n',velocity);
% fprintf(fid, '\n');

fprintf(fid, 'a\n');
fprintf(fid, 'a %2.2f\n',alpha);  
% fprintf(fid, '\n');

fprintf(fid, 'd1\n');
fprintf(fid, 'd1 %2.2f\n',elevator*5); 

%Run the Case
fprintf(fid, '%s\n',   'x'); 

%Save the st data
fprintf(fid, '%s\n',   'st'); 
fprintf(fid, '%s%s\n',basename,'.st');   
%Save the sb data
fprintf(fid, '%s\n',   'sb');
fprintf(fid, '%s%s\n',basename,'.sb');

%Drop out of OPER menu
fprintf(fid, '%s\n',   '');

%Switch to MODE menu
fprintf(fid, '%s\n',   'MODE');
fprintf(fid, '%s\n',   'n');
%Save the eigenvalue data
fprintf(fid, '%s\n',   'w');
fprintf(fid, '%s%s\n', basename,'.eig');   %File to save to

%Exit MODE Menu
fprintf(fid, '\n');     
%Quit Program
fprintf(fid, 'Quit\n'); 

%Close File
fclose(fid);