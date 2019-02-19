% Plot stacked state transition diagram, and create GIF demonstrating
% convergence of solution DNAs.
% Repo: https://github.com/mimocha/ga-logic-circuit
% Copyright (c) 2019 Chawit Leosrisook

%% ===== DEFINE VARIABLES ===== %%

% X & Y coordinates for arranging state transition diagram in a circle
theta = (0:2*pi/64:2*pi);
theta(65) = [];
x = cos(theta);
y = sin(theta);

% CA Neighbor & Color Definition
nb = 3;
c = 4;

%% ===== CALCULATE GRAPH ===== %%

% Row = Generation Count
row = 200;
% Column = Population Size
column = 100;

% Creates empty Row-by-Column array of state transition diagrams
G = repelem({digraph}, row, column);

% 'Ginfo' is a cell array of strings.
% Each cell contains 1 individual's DNA from 1 Generation
% 'Ginfo' is available in 'convergence_analysis.mat'

% Generate a state transition diagram for each DNA
for i = 1:row
	for j = 1:column
		G{i,j} = std (Ginfo{i,j+1}, nb, c);
	end
end

%% ===== ANIMATE STATE TRANSITION DIAGRAM ===== %%

h = figure(1);
filename = 'Convergence.gif';

% Nodes are unlabeled by default, due to MATLAB's afwul formatting.
% Node labels can be added, with the file 'nodeLabel.mat'
% The file is a cell array of strings.
% Each file contains a base-4 value of each node, in MSB format.

for i = 1:row
	% Clear
	clf;
	hold on;
	
	% Plot State Transition Diagram
	for j = 1:column
		plot(G{i,j}, 'xdata', x, 'ydata', y, 'Marker', 'o', 'NodeColor', 'b', ...
			'Nodelabel', {}, 'EdgeColor', 'b', 'EdgeAlpha', 0.025);
	end
	
	% Format Graph
	axis tight manual;
	xlim([-1.2 1.2]);
	ylim([-1.2 1.2]);
	yticks([]);
	xticks([]);
	title({'Generation ' i});
	drawnow;
	
	% Save to Gif
	frame = getframe(h);
	im = frame2im(frame);
	[imind,cm] = rgb2ind(im,256);
	
	% Write to the GIF File 
	if i == 1
		imwrite(imind,cm,filename,'gif', 'Loopcount',inf); 
	else
		imwrite(imind,cm,filename,'gif','WriteMode','append'); 
	end
end

%% ===== ANIMATE FITNESS GRAPH ===== %%

h = figure(2);
clf;
filename = 'Graph.gif';

subplot(2,1,1);
hold on;
grid on;
	f1 = plot([0 0], [0 0]);
	f2 = plot([0 0], [0 0]);
	f3 = plot([0 0], [0 0]);
ylim([0 10000])
xlim([0 200])
xlabel('Generation');
title('Fitness Graph');
legend ('Average', 'Median', 'Maximum', 'location', 'southeast');

subplot(2,1,2);
hold on;
grid on;
	g1 = plot([0 0], [0 0]);
	g2 = plot([0 0], [0 0]);
	g3 = plot([0 0], [0 0]);
ylim([0 4096])
xlim([0 200])
xlabel('Generation');
title('Efficiency Graph');
legend ('Average', 'Median', 'Maximum', 'location', 'east');

% 'Stats' is a table of fitness / efficiency data
% Each table column contains some statistics of a single generation.
% 'Stats' is available in 'convergence_analysis.mat'

for i = 1:row
	% Draw Fitness Graph
	set (f1, 'xdata', Stats.generation(1:i) ,'ydata', Stats.fitaverage(1:i));
	set (f2, 'xdata', Stats.generation(1:i) ,'ydata', Stats.fitmedian(1:i));
	set (f3, 'xdata', Stats.generation(1:i) ,'ydata', Stats.fitmaximum(1:i));
	
	% Draw Efficiency Graph
	set (g1, 'xdata', Stats.generation(1:i) ,'ydata', Stats.gateaverage(1:i));
	set (g2, 'xdata', Stats.generation(1:i) ,'ydata', Stats.gatemedian(1:i));
	set (g3, 'xdata', Stats.generation(1:i) ,'ydata', Stats.gatemaximum(1:i));
	
	drawnow
	
	% Save to Gif
	frame = getframe(h);
	im = frame2im(frame);
	[imind,cm] = rgb2ind(im,256);
	
	% Write to the GIF File 
	if i == 1
		imwrite(imind,cm,filename,'gif', 'Loopcount',inf); 
	else
		imwrite(imind,cm,filename,'gif','WriteMode','append'); 
	end
end







