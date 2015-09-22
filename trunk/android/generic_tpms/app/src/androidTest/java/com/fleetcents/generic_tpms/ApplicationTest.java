package com.fleetcents.generic_tpms;

import android.app.Application;
import android.os.Debug;
import android.test.ApplicationTestCase;

import java.nio.ByteBuffer;
import java.sql.SQLOutput;
import java.util.concurrent.ArrayBlockingQueue;

/**
 * <a href="http://d.android.com/tools/testing/testing_android.html">Testing Fundamentals</a>
 */
public class ApplicationTest extends ApplicationTestCase<Application> {
    public ApplicationTest() {
        super(Application.class);
    }

  @Override
  public void setUp() throws Exception {
    super.setUp();

  }
  protected static void printReIm(float[] re, float[] im) {
    System.out.print("Re: [");
    for(int i=0; i<re.length; i++)
      System.out.print(((int)(re[i]*1000)/1000.0) + " ");

    System.out.print("]\nIm: [");
    for(int i=0; i<im.length; i++)
      System.out.print(((int)(im[i]*1000)/1000.0) + " ");

    System.out.println("]");
  }

}