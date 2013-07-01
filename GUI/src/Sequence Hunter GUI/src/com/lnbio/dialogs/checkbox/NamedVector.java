package com.lnbio.dialogs.checkbox;

import java.util.Vector;

@SuppressWarnings({ "serial", "rawtypes" })
public class NamedVector extends Vector {
  String name;

  public NamedVector(String name) {
    this.name = name;
  }

  @SuppressWarnings("unchecked")
public NamedVector(String name, Object elements[]) {
    this.name = name;
    for (int i = 0, n = elements.length; i < n; i++) {
      add(elements[i]);
    }
  }

  public String toString() {
    return "[" + name + "]";
  }
}