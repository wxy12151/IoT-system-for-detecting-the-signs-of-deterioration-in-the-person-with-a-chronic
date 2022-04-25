function [] = IoTvs

sensor=imread('sensorlayout.jpg');
data=readtable('dataset_new.csv','format','%{dd/MM/yyyy}D%{HH:mm:ss}D%q%q%q%q');

% Sensor List
cll=cell(90,1);
% Motion
for i=1:51
    cll(i)={strcat('M',num2str(i,'%02d'))};
end
% Item
for i=1:9
    cll(51+i)={strcat('I',num2str(i,'%02d'))};
end
% Door
for i=1:12
    cll(51+9+i)={strcat('D',num2str(i,'%02d'))};
end
% Touch
for i=1:3
    cll(51+9+12+i)={strcat('T',num2str(i,'%02d'))};
end
% Light
for i=1:17
    cll(51+9+12+3+i)={strcat('L',num2str(i,'%02d'))};
end
cll(end+1)={'AD1-A'};
cll(end+1)={'AD1-B'};
cll(end+1)={'AD1-C'};


% Type 1 to plot heatmap at nighttimes
night = input('Detect Night Wandering? yes:1/no:0 \n');
% Plot heatmap for each month (of 2009 only)
pic_index=1;
R1_room=zeros(1,12);
Bathroom=zeros(1,12);
Corridor=zeros(1,12);
Living_Room=zeros(1,12);
Kitchen=zeros(1,12);
for current_month=1:12
    month_start=append('01/',num2str(current_month,'%02d'),'/2009');
    if current_month==12
        month_end='01/01/2010';
    else
        month_end=append('01/',num2str(current_month+1,'%02d'),'/2009');
    end
    month_data=data(isbetween(data.Var1,datetime(month_start,'InputFormat','dd/MM/yyyy'),datetime(month_end,'InputFormat','dd/MM/yyyy')),:);
    if not(isempty(month_data))
        if night==1
            month_data_night_1=month_data(isbetween(month_data.Var2,datetime('23:00:00','InputFormat','HH:mm:ss'),datetime('23:59:59','InputFormat','HH:mm:ss')),:);
            month_data_night_2=month_data(isbetween(month_data.Var2,datetime('00:00:00','InputFormat','HH:mm:ss'),datetime('6:00:00','InputFormat','HH:mm:ss')),:);
            month_data=[month_data_night_1;month_data_night_2];
            counter=tabulate(month_data.Var3);
        elseif night ==0
            counter=tabulate(month_data.Var3);
        end
        str_var=counter(:,1);
        for i=1:length(cll)
            if not(ismember(cll(i),str_var))
                counter(end+1,:)={cll(i),0,0};
            end
        end

        new_table=table(cell2mat(counter(:,2)),'RowNames',string(counter(:,1)));
        
        % Sensor Locations
        c=zeros(57,74);
        % R1 room
        c(46,20)=new_table{'M35',1};
        c(52,20)=new_table{'M34',1};
        c(52,13)=new_table{'M33',1};
        c(46,13)=new_table{'M32',1};
        c(40,13)=new_table{'M31',1};
        c(42,13)=new_table{'T03',1};
        c(40,20)=new_table{'M36',1};
        c(36,22)=new_table{'M30',1};
        c(38,20)=new_table{'D03',1};
        c(35,17)=new_table{'L10',1};
        R1_room(current_month)= sum(sum(c));

        % Bathroom
        c(55,29)=new_table{'D06',1};
        c(53,29)=new_table{'M41',1};
        c(50,29)=new_table{'M40',1};
        c(44,29)=new_table{'M39',1};
        c(37,29)=new_table{'M38',1};
        c(33,29)=new_table{'D05',1};
        c(36,31)=new_table{'L11',1};
        c(47,29)=new_table{'L12',1};
        c(47,33)=new_table{'L13',1};
        c(53,33)=new_table{'L14',1};
        c(55,33)=new_table{'L15',1};
        c(53,30)=new_table{'L16',1};
        c(32,28)=new_table{'M37',1};
        Bathroom(current_month)=sum(sum(c))-R1_room(current_month);
        
        % corridor
        c(33,17)=new_table{'L07',1};
        c(32,22)=new_table{'M29',1};
        c(32,15)=new_table{'M28',1};
        c(32,6)=new_table{'M27',1};
        c(27,22)=new_table{'M42',1};
        c(46,42)=new_table{'M26',1};
        Corridor(current_month)=sum(sum(c))-R1_room(current_month)-Bathroom(current_month);

        % Living Room 
        c(6,43)=new_table{'M04',1};
        c(6,51)=new_table{'M05',1};
        c(6,60)=new_table{'M11',1};
        c(6,69)=new_table{'M12',1};
        c(6,64)=new_table{'D02',1};
        c(13,43)=new_table{'M03',1};
        c(13,51)=new_table{'M06',1};
        c(13,60)=new_table{'M10',1};
        c(13,69)=new_table{'M13',1};
        c(15,51)=new_table{'T01',1};
        c(15,58)=new_table{'I08',1};
        c(17,58)=new_table{'I09',1};
        c(21,43)=new_table{'M02',1};
        c(21,51)=new_table{'M07',1};
        c(21,60)=new_table{'M09',1};
        c(21,69)=new_table{'M14',1};
        c(10,39)=new_table{'I03',1};
        c(16,39)=new_table{'I05',1};
        c(23,58)=new_table{'M08',1};
        c(27,45)=new_table{'L05',1};
        c(27,49)=new_table{'M01',1};
        c(27,58)=new_table{'L04',1};
        c(27,62)=new_table{'L03',1};
        c(27,66)=new_table{'M15',1};
        c(30,49)=new_table{'M23',1};
        Living_Room(current_month)=sum(sum(c))-R1_room(current_month)-Bathroom(current_month)-Corridor(current_month);
        
        % Kitchen
        c(30,66)=new_table{'M16',1};
        c(32,52)=new_table{'D12',1};
        c(31,55)=new_table{'D08',1};
        c(33,55)=new_table{'D09',1};
        c(35,55)=new_table{'D10',1};
        c(34,70)=new_table{'AD1-A',1};
        c(37,49)=new_table{'M22',1};
        c(37,66)=new_table{'M17',1};
        c(39,66)=new_table{'T02',1};
        c(43,53)=new_table{'L02',1};
        c(44,49)=new_table{'M21',1};
        c(44,57)=new_table{'M19',1};
        c(44,66)=new_table{'M18',1};
        c(47,70)=new_table{'AD1-B',1};
        c(48,70)=new_table{'AD1-C',1};
        c(49,40)=new_table{'L06',1};
        c(51,57)=new_table{'M20',1};
        c(53,42)=new_table{'M25',1};
        c(53,49)=new_table{'M24',1};
        c(54,46)=new_table{'D01',1};
        c(56,45)=new_table{'L01',1};
        c(54,53)=new_table{'L17',1};
        c(54,67)=new_table{'D11',1};
        c(54,70)=new_table{'M51',1};
        c(38,71)=new_table{'I07',1};
        c(38,72)=new_table{'D07',1};
        c(39,71)=new_table{'I01',1};
        c(40,71)=new_table{'I02',1};
        c(39,72)=new_table{'I06',1};
        c(40,72)=new_table{'I04',1};
        Kitchen(current_month)=sum(sum(c))-R1_room(current_month)-Bathroom(current_month)-Corridor(current_month)-Living_Room(current_month);
        
        % R2 room
        c(27,15)=new_table{'M43',1};
        c(27,17)=new_table{'D04',1};
        c(23,16)=new_table{'M44',1};
        c(21,17)=new_table{'L08',1};
        c(17,15)=new_table{'M50',1};
        c(10,15)=new_table{'M49',1};
        c(5,15)=new_table{'M48',1};
        c(5,6)=new_table{'M47',1};
        c(10,6)=new_table{'M46',1};
        c(17,6)=new_table{'M45',1};
        
        % Plot heatmap on the floorplan
        figure('Name','Heat Map')
        sen_im=image([0 945],[0 574],sensor);
        hold on
        c_im=image([0 740],[0 570],c,'CDataMapping','scaled');
        set(c_im,'alphadata',~~c);
        title(append('Month ',num2str(current_month),': Heat Map'));
        xlim([0 942]);
        ylim([0 576]);
        colormap(cool);
        colorbar;
        caxis([0 15000]);
        
        % Generate a GIF to observe activity changes intuitively
        [A,map] = rgb2ind(frame2im(getframe),256);
        if pic_index == 1
            imwrite(A,map,'Heatmap.gif','gif','Loopcount',inf,'DelayTime',2);
        else
            imwrite(A,map,'Heatmap.gif','gif','WriteMode','append','DelayTime',2);
        end
        pic_index = pic_index + 1;
    end
end

% Activity statistics
figure('Name','Activity statistics')
plot(R1_room);
hold on
plot(Living_Room,'-.');
hold on
plot(Bathroom,'--');
hold on
plot(Kitchen,':');
hold on
plot(Corridor,'-');
xlabel('Month');
ylabel('Count');
legend('R1 Room','Living Room','Bathroom','Kitchen','Corridor')

end
