package body Mhmm is

   function Get_Sum_and_Product(X : Integer; Y : Integer) return Sum_Product is
      Result : Sum_Product;
   begin
      Result.Sum := X + Y;
      Result.Product := X * Y;
      return Result;
   end Get_Sum_and_Product;

end Mhmm;
