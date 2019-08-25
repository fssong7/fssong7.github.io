function [alpha,elevator,Cl,Cd,Cm] = postProcessing()
    fid  = fopen('newData1.st','r');
    text = textscan(fid,'%s','Delimiter','');
    text = text{1};
    fid  = fclose(fid);
    
    for row = 1:30
        k = strfind(text,'Alpha');
        if k{row}~=0
            y = extractBetween(text{row},k{row}+9,k{row}+16);
            alpha = str2double(y);
        end
        row = row + 1;
    end
     for row = 1:30
        k = strfind(text,'elevator');
        if k{row}~=0
            y = extractBetween(text{row},k{row}+19,k{row}+26);
            elevator = str2double(y);
        end
        row = row + 1;
    end
    for row = 1:30
        k = strfind(text,'CLtot');
        if k{row}~=0
            y = extractBetween(text{row},k{row}+9,k{row}+16);
            Cl = str2double(y);
        end
        row = row + 1;
    end
    for row = 1:30
        k = strfind(text,'CDtot');
        if k{row}~=0
            y = extractBetween(text{row},k{row}+9,k{row}+16);
            Cd = str2double(y);
        end
        row = row + 1;
    end
    for row = 1:30
        k = strfind(text,'Cmtot');
        if k{row}~=0
            y = extractBetween(text{row},k{row}+9,k{row}+16);
            Cm = str2double(y);
        end
        row = row + 1;
    end

%  fprintf('The angle of attack is %2.5f degrees\n',alpha);
fprintf('The elevator is %2.5f degrees\n',elevator);
% fprintf('The coefficient of lift is %2.5f\n',Cl);
% fprintf('The coefficient of drag is %2.5f\n',Cd);
% fprintf('The coefficient of moment is %2.5f\n',Cm);


