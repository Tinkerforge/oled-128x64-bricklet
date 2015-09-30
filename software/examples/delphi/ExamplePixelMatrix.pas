program ExamplePixelMatrix;

{$ifdef MSWINDOWS}{$apptype CONSOLE}{$endif}
{$ifdef FPC}{$mode OBJFPC}{$H+}{$endif}

uses
  Math, SysUtils, IPConnection, BrickletOLED128x64;

const
  SCREEN_WIDTH = 128;
  SCREEN_HEIGHT = 64;

type
  TPixelMatrix = array[0..(SCREEN_HEIGHT - 1), 0..(SCREEN_WIDTH - 1)] of boolean;
  TExample = class
  private
    ipcon: TIPConnection;
    oled: TBrickletOLED128x64;
  public
    procedure DrawMatrix(pixels: TPixelMatrix);
    procedure Execute;
  end;

const
  HOST = 'localhost';
  PORT = 4223;
  UID = 'XYZ'; { Change to your UID }

var
  e: TExample;

procedure TExample.DrawMatrix(pixels: TPixelMatrix);
var
  i, j, k, l: integer;
  page: byte;
  column: array[0..7, 0..(SCREEN_WIDTH - 1)] of byte;
  column_write: array[0..63] of byte;
begin
  for i := 0 to 7 do begin
    for j := 0 to SCREEN_WIDTH - 1 do begin
      page := 0;
      for k := 0 to 7 do begin
        if (pixels[(i*8) + k, j]) then begin
          page := page or (1 << k);
        end;
      end;

      column[i][j] := page;
    end;
  end;

  oled.NewWindow(0, SCREEN_WIDTH - 1, 0, 7);

  for i := 0 to 7 do begin
    l := 0;
    for j := 0 to Floor(SCREEN_WIDTH/2) - 1 do begin
      column_write[l] := column[i, j];
      l := l + 1;
    end;

    oled.Write(column_write);

    l := 0;
    for k := Floor(SCREEN_WIDTH/2) to SCREEN_WIDTH - 1 do begin
      column_write[l] := column[i, k];
      l := l + 1;
    end;

    oled.Write(column_write);
  end;
end;

procedure TExample.Execute;
var
  pixel_matrix: TPixelMatrix;
  w, h: integer;
begin
  { Create IP connection }
  ipcon := TIPConnection.Create;

  { Create device object }
  oled := TBrickletOLED128x64.Create(UID, ipcon);

  { Connect to brickd }
  ipcon.Connect(HOST, PORT);
  { Don't use device before ipcon is connected }

  { Clear display }
  oled.ClearDisplay;

  { Pixel matrix with all pixels turned off }
  for h := 0 to SCREEN_HEIGHT - 1 do begin
    for w := 0 to SCREEN_WIDTH - 1 do begin
      pixel_matrix[h, w] := false;
    end;
  end;

  { Draw check pattern }
  for w := 0 to SCREEN_WIDTH - 1 do begin
    for h := 0 to SCREEN_HEIGHT - 1 do begin
      if (Floor(w/5) mod 2 = 0) then begin
        pixel_matrix[h, w] := true;
      end;
      if (Floor(h/5) mod 2 = 0) then begin
        pixel_matrix[h, w] := true;
      end;
    end;
  end;

  e.DrawMatrix(pixel_matrix);

  WriteLn('Press key to exit');
  ReadLn;
  ipcon.Destroy; { Calls ipcon.Disconnect internally }
end;

begin
  e := TExample.Create;
  e.Execute;
  e.Destroy;
end.
