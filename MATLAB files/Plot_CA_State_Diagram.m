% Plot multiple state diagram

theta = (0:2*pi/64:2*pi);
theta(65) = [];
x = cos(theta);
y = sin(theta);

nb = 3;
c = 4;

% label = {'FALSE', 'NOR', ...
% 	'A & !B', '!B', 'B & !A', '!A', ...
% 	'XOR', 'NAND', 'AND', 'XNOR', ...
% 	'A', 'A + !B', 'B', 'B + !A', ...
% 	'OR', 'TRUE', ...
% 	'NAND LATCH', 'NOR LATCH', 'RANDOM - LABELED'};

%% ===== CALCULATE GRAPH ===== %%

row = 200;
column = 100;

G = repelem({digraph}, row, column);

for i = 1:row
	for j = 1:column
		G{i,j} = std (Ginfo{i,j+1}, nb, c);
	end
end

%% ===== PLOT FIGURE 1 ===== %%

% figure(1);
% 
% width = 0.3;
% height = 0.3;
% idx = [0.025 0.35 0.675 0.025 0.35 0.675 0.025 0.35 0.675];
% idy = [0.675 0.675 0.675 0.35 0.35 0.35 0.025 0.025 0.025];
% 
% for i = 1:8
% 	subplot(3,3,i);
% 	set(gca, 'units', 'normalized', 'position', [idx(i) idy(i) width height])
% 	hold on;
% 	
% 	for j = 1:solution_count
% 		plot(G{i,j}, 'XData', x, 'YData', y, ...
% 		'Marker', 'o', 'NodeColor', 'b', 'Nodelabel', {}, ...
% 		'EdgeColor', 'b', 'EdgeAlpha', 0.025);
% 	end
% 	
% 	yticks([]);
% 	xticks([]);
% 	title(label{i});
% end
% 
% i = 9;
% 
% subplot(3,3,i);
% set(gca, 'units', 'normalized', 'position', [idx(i) idy(i) width height])
% hold on;
% 
% for j = 1:solution_count
% 	plot(G{11,j}, 'XData', x, 'YData', y, ...
% 	'Marker', 'o', 'NodeColor', 'b', 'Nodelabel', {}, ...
% 	'EdgeColor', 'b', 'EdgeAlpha', 0.025);
% end
% 
% yticks([]);
% xticks([]);
% title(label{i});

%% ===== PLOT FIGURE 2 ===== %%

% figure(1);
% 
% for i = 18
% 	clf;
% 	hold on;
% 	
% 	for j = 1:solution_count
% 		plot(G{i+1,j}, 'XData', x, 'YData', y, ...
% 		'Marker', 'o', 'NodeColor', 'b', 'Nodelabel', {}, ...
% 		'EdgeColor', 'b', 'EdgeAlpha', 0.025);
% 	end
% 	
% 	plot(G{i+1,j}, 'XData', x, 'YData', y, ...
% 		'Marker', 'o', 'NodeColor', 'b', 'Nodelabel', nodeLabelsimp, ...
% 		'EdgeColor', 'b', 'EdgeAlpha', 0.025);
% 	
% 	yticks([]);
% 	xticks([]);
% 	title(label{i+1});
% 	
% 	saveas(gcf, label{i+1}, 'fig');
% 	saveas(gcf, label{i+1}, 'png');
% end

%% ===== %%

% label = {'FALSE', 'NOR', ...
% 	'A & !B', '!B', 'B & !A', '!A', ...
% 	'XOR', 'NAND', 'AND', 'XNOR', ...
% 	'A', 'A + !B', 'B', 'B + !A', ...
% 	'OR', 'TRUE', ...
% 	'NAND LATCH 1', 'NAND LATCH 2', 'NAND LATCH 3', ...
% 	'NOR LATCH 1', 'NOR LATCH 2', 'NOR LATCH 3', ...
% 	'Sequential Boolean NOR'};
% 
% List = { ... % 16 Boolean functions
% 	'0030131020030300000101030103033321333200011001030323123330333000', ...
% 	'1112310302033013300011330321010001300101020233003100302310201333', ...
% 	'1211130003330103303011310031313132310103003033010330310100020231', ...
% 	'2200221303031331331011111300002310322101001103333213001000003021', ...
% 	'3033020222101233100011232030110100003110020002011331310103303000', ...
% 	'0102311201333301101011113311000303311132001332300330220133311133', ...
% 	'3013230033203133003221013313013121300303302300030333303330033301', ...
% 	'1002222101302313113211110000211303310001200003300303000210101201', ...
% 	'0011323110202113010320311033030002233013000133100133013003233010', ...
% 	'1333033311302303013231310130233100000103332033331323332330101123', ...
% 	'0130100231132300110201010323000013332130031033020330302302310033', ...
% 	'0000212313230223120011112101130210101323333330133030001322310001', ...
% 	'1131020211002321222211101130323132113212131321112332212213302103', ...
% 	'0330230333032111013021110131110313021033303033332133310001012330', ...
% 	'3330103330320031330033121213112111030111101203003333312111101231', ...
% 	'2000300103023020223022300002020010033103000202032131020302103201', ...
% 	... % 3 NAND Latches
% 	'2023331301333313231333320020333322331322232030300013221033221321', ...
% 	'2030303032023312230303022323010320130333202210331032331313233233', ...
% 	'2023312101300002110021131031012111303121022133130300311320100200', ...
% 	... % 3 NOR Latches
% 	'2132330201331002200013100312031000302103332212032331332330303331', ...
% 	'1320212133310013032130310320012110021131122223132022313311233312', ...
% 	'0030033100031333323030030322323320210313300301031002333200133123', ...
% 	... % 'Sequential' NOR Boolean Gate
% 	'0003330002200100120013300333023032100030203103220200330313123000', ...
% };

figure(1);

for i = 1:row
	clf;
	
	plot(G{i}, 'xdata', x, 'ydata', y, 'Marker', 'o', ...
		'Nodelabel', {}, 'EdgeColor', 'b', 'EdgeAlpha', 0.025);
	
	yticks([]);
	xticks([]);
	title({'Generation ' i});
	
	saveas(gcf, strcat(label{i}, 'STD'), 'fig');
	saveas(gcf, label{i}, 'png');
end

