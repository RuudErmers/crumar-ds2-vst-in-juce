unit Unit1;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls;

type
  TForm1 = class(TForm)
    MemoIn1: TMemo;
    MemoConsts: TMemo;
    Button1: TButton;
    MemoOut: TMemo;
    MemoIn2: TMemo;
    procedure Button1Click(Sender: TObject);
  private
    { Private declarations }
    mins,maxs,defs:array[0..50] of integer;
    procedure ProcessMinMax;
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

implementation

{$R *.dfm}

procedure TForm1.ProcessMinMax;
VAR i,index, p:integer;
    s,sindex,smin,smax,sdef:string;
    function Lookup(s:string):integer;
    VAR i:integer;
    begin
      for i:=0 to MemoConsts.Lines.Count-1 do
        if pos(s,MemoConsts.Lines[i])<>0 then begin result:=i; exit; end;
    end;
begin
  for i:=0 to MemoIn2.LInes.Count-1 do
  begin
    s:=MemoIn2.LInes[i];   //   AddParameter(PARMOSC1TYPE,'OSC1 Wave',0,3,0,'','Square PW;Square;Triangle;Saw');
    p:=pos('(',s);

    s:=Copy(s,p+1);        // PARMOSC1TYPE,'OSC1 Wave',0,3,0,'','Square PW;Square;Triangle;Saw');
    p:=pos(',',s);
    sindex:=Copy(s,1,p-1); // PARMOSC1TYPE

    s:=Copy(s,p+1);        // 'OSC1 Wave',0,3,0,'','Square PW;Square;Triangle;Saw');
    p:=pos(',',s);
    // skip this

    s:=Copy(s,p+1);        // 0,3,0,'','Square PW;Square;Triangle;Saw');
    p:=pos(',',s);
    smin:=Copy(s,1,p-1);

    s:=Copy(s,p+1);        // 3,0,'','Square PW;Square;Triangle;Saw');
    p:=pos(',',s);
    smax:=Copy(s,1,p-1);

    s:=Copy(s,p+1);        // 3,0,'','Square PW;Square;Triangle;Saw');
    p:=pos(',',s);
    sdef:=Copy(s,1,p-1);
    index:=LookUp(sindex);
    mins[index]:=strtoint(smin);
    maxs[index]:=strtoint(smax);

  end;
end;


procedure TForm1.Button1Click(Sender: TObject);
VAR i,l,t,w,h,v,_in:integer;
    leftoff,topoff,left,top,width,height,index:integer;
    c,caption,sh,shape,s:string;
    InElement:boolean;
    function iattrib(sub,s:string): integer;
    VAR p:integer;
    begin
      result:=-1;
      if (pos(sub,s)<>0) and (pos('Font',s)=0) then
      begin
        p:=pos('=',s);
        if p<>0 then
          result:=StrToIntDef(Copy(s,p+2),-1);
      end;
    end;
    function sattrib(sub,s:string): string;
    VAR p:integer;
    begin
      result:='';
      if pos(sub,s)<>0 then
      begin
        p:=pos('=',s);
        if p<>0 then
          result:=Copy(s,p+2);
      end;
    end;
    procedure NewElement;
    begin
      shape:='';
      InElement:=true;
    end;
    procedure EndElement;
    VAR s:string;
        p:integer;
    begin
      s:=MemoConsts.Lines[index];
      p:=pos(' ',s);
      s:=Copy(s,p+1);
      p:=pos(' ',s);
      s:=Copy(s,1,p-1);

      if shape<>'' then
        MemoOut.Lines.Add(
        // 	addViewItem(id, shape,x,y,w,h, viewItems[id].min, viewItems[id].max,1);
         '  addViewItem('+s+','+shape+','+inttostr(Left+leftoff)+','+inttostr(Top+Topoff)+','+inttostr(Width)+','+inttostr(Height)+','
                        +inttostr(mins[index])+','+inttostr(maxs[index])+',1);');
      InElement:=false;
    end;
begin
  ProcessMinMax;
  InElement:=false;
  for i:=0 to MemoIn1.Lines.Count-1 do
  begin
    s:=MemoIn1.Lines[i];
    if pos('TRMCElement',s)<>0 then NewElement;
    if InElement then
    begin
      l:=iattrib('Left',s); if l<>-1 then Left:=l;
      t:=iattrib('Top',s); if t<>-1 then Top:=t;
      w:=iattrib('Width',s); if w<>-1 then Width:=w;
      h:=iattrib('Height',s); if h<>-1 then Height:=h;
      _in:=iattrib('Index',s); if _in<>-1 then Index:=_in;
      sh:=sattrib('Options.Shape',s); if sh<>'' then shape:=sh;
      c:=sattrib('Caption',s); if c<>'' then caption:=c;
      v:=iattrib('Value',s); if v<>-1 then EndElement;
    end
    else
    begin
      if pos('TRMCEmptyPanel',s)>0 then
      begin
        leftoff:=iattrib('Left',MemoIn1.Lines[i+1]);
        topoff:=iattrib('Top',MemoIn1.Lines[i+2]);
      end;
    end;
  end;
  MemoOut.SelectAll;
  MemoOut.CopyToClipboard;
end;

end.


