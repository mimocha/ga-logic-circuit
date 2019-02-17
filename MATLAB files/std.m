% CA State Transition Diagram
% Takes a DNA String as input, returns a directional graph as output

function G = std (DNA_String, nb, c)

%% ============================================================== %%
% Error Catch
% ==============================================================

% Error Catch
if ( length(DNA_String) ~= c^nb )
	disp('Error: DNA Length Mismatch');
	return;
end



%% ============================================================== %%
% LOAD DNA STRING
% ==============================================================

% DNA Format: (1) LSB -> MSB (64)
% DNA_String = '01011010';
DNA = str2num(DNA_String(:));



%% ============================================================== %%
% CREATE EMPTY GRAPH
% ==============================================================

% Node Count = Color^Neighbor (CA State Count)
nodecount = c^nb;

% Node Name : LSB first
% Empty Directional Graph
if ((nb == 3) && (c == 4))
	nodename = { ...
		'000', '100', '200', '300', '010', '110', '210', '310', ...
		'020', '120', '220', '320', '030', '130', '230', '330', ...
		'001', '101', '201', '301', '011', '111', '211', '311', ...
		'021', '121', '221', '321', '031', '131', '231', '331', ...
		'002', '102', '202', '302', '012', '112', '212', '312', ...
		'022', '122', '222', '322', '032', '132', '232', '332', ...
		'003', '103', '203', '303', '013', '113', '213', '313', ...
		'023', '123', '223', '323', '033', '133', '233', '333' };
	G = digraph (zeros(nodecount), nodename);
else
	nodename = { '000', '100', '010', '110', '001', '101', '011', '111' };
	G = digraph (zeros(nodecount), nodename);
end




%% ============================================================== %%
% NODE TRAVERSAL FLAG
% ==============================================================

% Flag for whether or not each node has been traversed to before
traversed = repelem(false, nodecount);



%% ============================================================== %%
% CREATE STATE TRANSITION GRAPH
% ==============================================================

% Prepare Cell Neighborhood
% Neighborhood is in LSB format -> NB[1] < NB[2] < NB[3]
NB = zeros(1,3);
NB_Next = NB;

weight = 1;

% Repeat for all starting state 'node'
for node = 1:nodecount
	
	% ===== If traversed, skip node ===== %
	if (traversed(node) == true)
		continue;
	end
	
	% ===== If not traversed, check node ===== %
	while (true)
		% Set current node as the Neighborhood
		% Converting Nodename string into int array
		% Should change this
		s = cell2mat(nodename(node));
		NB(:) = str2num(s(:));

		% Convert Neighborhood (int array) to node (int decimal)
		node = 1 + NB(1) + NB(2)*c + NB(3)*c^2;
		
		% Apply CA Rule to generate next NB
		NB_Next = ca_rule (NB, DNA, nb, c);
		node_next = 1 + NB_Next(1) + NB_Next(2)*c + NB_Next(3)*c^2;
		
		% Connect current node to next node on graph - if not yet connected
		idxEdge = findedge(G, node, node_next);
		if (idxEdge == 0)
			G = addedge(G, node, node_next, weight);
		end
		
		% Set current node as traversed
		traversed(node) = true;

		% Set current node as next node
		node = node_next;

		% Repeat until reached a traversed node
		if (traversed(node) == true)
			break;
		end
	end
end

end
