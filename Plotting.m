% Lê os dados do CSV
dados = readtable('resultados.csv');

% Extrai as colunas
tamanho = dados.Tamanho;
semHeap = dados.SemHeap;
comHeap = dados.ComHeap;

% Cria o gráfico
figure;
plot(tamanho, semHeap, '-o', 'LineWidth', 2, 'DisplayName', 'Fila sem HEAP');
hold on;
plot(tamanho, comHeap, '-s', 'LineWidth', 2, 'DisplayName', 'Fila com HEAP');
hold off;

% Personalização do gráfico
title('Comparação de número de comparações: Fila com vs. sem HEAP');
xlabel('Tamanho da entrada (números inseridos/removidos)');
ylabel('Número de comparações');
legend('Location', 'northwest');
grid on;
