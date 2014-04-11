clear all;
% rogers_patterns;    

% labels;
% H = zeros(48-1, 20);
load('tyler_patterns.mat')
H = zeros(16-1, 20);
% for l = 1:20,
    l = 1;

% l = 1;
    %     clear H;
    %     clear T;
    %     figure;

    eval(['tyler_reccurent_' num2str(l-1) ';']);

%     eval(['bptt_category_patterns_2_all_clamped_1_' num2str(l-1) ';']);

    %eval(['bptt_category_patterns_6_' num2str(l-1) ';']);

    Z = linkage(pdist(X, 'euclidean'), 'average');
        f = figure('units','normalized','position',[.1 .1 700 800])


%     H(:, l) = dendrogram(Z, 0, 'colorthreshold', 0 , 'labels', S, 'orientation', 'right');
    H(:, l) = dendrogram(Z, 0, 'colorthreshold', 0 , 'labels', labels, 'orientation', 'right');
    set(H(:, l), 'LineWidth', 2);
    xlabel('Euclidean distance');
%     eval(['title ''Internal Representations Given Name Input at ' num2str((l-1)*100.0/20.0) '%''']);
   title 'Internal Representations'
% title 'Visual Input Patterns'
%     set(0,'defaultaxesfontname', 'Times');
%     set(0,'defaulttextfontname', 'Times');
%     saveas(f, eval(['''dendrogram_rogers_pattern_wise_' num2str(l-1) '.pdf''']), 'pdf');
    saveas(f, eval(['''tyler_recurrent_' num2str(l-1) '.pdf''']), 'pdf');

%cophenet(Z,pdist(X))
% end
