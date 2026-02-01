declare procedure printf no check;

create proc hello()
begin
  call printf("Hello, world\n");
end;
