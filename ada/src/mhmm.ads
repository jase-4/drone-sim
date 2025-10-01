package Mhmm is

   type Sum_Product is record
      Sum    : Integer;
      Product: Integer;
   end record;

   function Get_Sum_and_Product(X : Integer; Y : Integer) return Sum_Product;

end Mhmm;