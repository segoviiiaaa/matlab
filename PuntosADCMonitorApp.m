classdef PuntosADCMonitorApp < matlab.apps.AppBase
    % PuntosADCMonitorApp: Aplicación App Designer para monitoreo ADC de Arduino en tiempo real

    % Properties that correspond to app components
    properties (Access = public)
        UIFigure          matlab.ui.Figure
        UIAxes            matlab.ui.control.UIAxes
        StartButton       matlab.ui.control.Button
        StopButton        matlab.ui.control.Button
        ExportButton      matlab.ui.control.Button
        VoltageValueLabel matlab.ui.control.Label
        MassValueLabel    matlab.ui.control.Label
    end

    properties (Access = private)
        a                 % Objeto Arduino
        DataTimer         timer  % Timer para lectura periódica
        voltajes = []     % Vector de voltajes
        masas = []        % Vector de masas calculadas
        t0                % Tiempo inicial
        voltCal          % Puntos de calibración: voltajes
        massCal          % Puntos de calibración: masas
    end

    methods (Access = private)

        function startupFcn(app)
            % Conectar al Arduino (ajusta el puerto si es necesario)
            app.a = arduino("COM9", "Uno");

            % Calibración voltaje -> masa (kg)
            app.voltCal = [-0.004 0.387 0.892 1.416 1.962 2.51];
            app.massCal = [0 0.101 0.202 0.300 0.402 0.502];

            % Configurar timer de muestreo cada 0.1 s
            app.t0 = tic;
            app.DataTimer = timer('ExecutionMode','fixedRate', ...
                                  'Period', 0.1, ...
                                  'TimerFcn', @(~,~) readAndPlot(app));
        end

        function readAndPlot(app)
            % Leer voltaje en A0
            v = readVoltage(app.a, 'A0');

            % Calcular masa mediante interpolación
            m = interp1(app.voltCal, app.massCal, v, 'linear', 'extrap');

            % Guardar datos
            app.voltajes(end+1) = v;
            app.masas(end+1) = m;

            % Actualizar etiquetas con valores actuales
            app.VoltageValueLabel.Text = sprintf('Voltaje: %.3f V', v);
            app.MassValueLabel.Text = sprintf('Masa: %.1f g', m*1000);

            % Graficar puntos de correlación
            scatter(app.UIAxes, app.masas, app.voltajes, 'filled');
            xlabel(app.UIAxes, 'Masa (kg)');
            ylabel(app.UIAxes, 'Voltaje (V)');
            grid(app.UIAxes, 'on');
            xlim(app.UIAxes, [0 1]);
            ylim(app.UIAxes, [0 5]);
        end

        % Botón Start
        function StartButtonPushed(app, ~)
            start(app.DataTimer);
        end

        % Botón Stop
        function StopButtonPushed(app, ~)
            stop(app.DataTimer);
        end

        % Botón Exportar
        function ExportButtonPushed(app, ~)
            [file, path] = uiputfile('data.txt', 'Guardar datos como');
            if isequal(file, 0)
                return;
            end
            T = table(app.masas', app.voltajes', 'VariableNames', {'Masa_kg', 'Voltaje_V'});
            writetable(T, fullfile(path, file), 'Delimiter', '\t');
            uialert(app.UIFigure, 'Datos exportados correctamente', 'Exportación');
        end
    end

    methods (Access = public)

        % Constructor
        function app = PuntosADCMonitorApp
            createComponents(app);
            registerApp(app, app.UIFigure);
            runStartupFcn(app, @(app) startupFcn(app));
        end

        % Destructor
        function delete(app)
            if isvalid(app.DataTimer)
                stop(app.DataTimer);
                delete(app.DataTimer);
            end
            if ~isempty(app.a)
                clear app.a;
            end
            delete(app.UIFigure);
        end
    end

    methods (Access = private)

        % Crear componentes de la GUI
        function createComponents(app)
            app.UIFigure = uifigure('Position', [100 100 600 400], 'Name', 'Puntos ADC Monitor App');

            app.UIAxes = uiaxes(app.UIFigure, ...
                'Position', [50 100 500 250], ...
                'XLim', [0 1], ...
                'YLim', [0 5]);

            % Etiquetas para valores actuales
            app.VoltageValueLabel = uilabel(app.UIFigure, ...
                'Position', [50 360 200 22], ...
                'Text', 'Voltaje: 0 V');

            app.MassValueLabel = uilabel(app.UIFigure, ...
                'Position', [300 360 200 22], ...
                'Text', 'Masa: 0 g');

            app.StartButton = uibutton(app.UIFigure, ...
                'Position', [50 30 100 40], ...
                'Text', 'Iniciar', ...
                'ButtonPushedFcn', @(btn,event) StartButtonPushed(app));

            app.StopButton = uibutton(app.UIFigure, ...
                'Position', [250 30 100 40], ...
                'Text', 'Detener', ...
                'ButtonPushedFcn', @(btn,event) StopButtonPushed(app));

            app.ExportButton = uibutton(app.UIFigure, ...
                'Position', [450 30 100 40], ...
                'Text', 'Exportar', ...
                'ButtonPushedFcn', @(btn,event) ExportButtonPushed(app));
        end
    end
end
