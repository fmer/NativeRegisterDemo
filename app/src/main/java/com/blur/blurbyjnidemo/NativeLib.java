package com.blur.blurbyjnidemo;

public class NativeLib {
         // Used to load the 'native-lib' library on application startup.
         static {
                  System.loadLibrary("native-lib");
         }

         /**
          * A native method that is implemented by the 'native-lib' native library,
          * which is packaged with this application.
          */
         public  native String stringFromJNI();
         public static native String stringFromJNI2();
}
