with Ada.Text_IO;  -- For output
use Ada.Text_IO;

procedure Mhm is

   type Sum_Product is record
      Sum    : Integer;
      Product: Integer;
   end record;

   function Get_Sum_and_Product(X : Integer; Y : Integer) return Sum_Product is
      Result : Sum_Product;
   begin
      Result.Sum := X + Y;
      Result.Product := X * Y;
      return Result;
   end Get_Sum_and_Product;

   A : Integer := 5;
   B : Integer := 3;
   Result : Sum_Product;

begin
   Result := Get_Sum_and_Product(A, B);
   Put_Line("Sum: " & Result.Sum'Image);  -- Output: Sum: 8
   Put_Line("Product: " & Result.Product'Image);  -- Output: Product: 15
end Mhm;