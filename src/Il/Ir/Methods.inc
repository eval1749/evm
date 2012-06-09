// -*- Mode: C++; -*-
// @(#)$Id$
//
// Evita VM IL IR Methods
//

#if !defined(DEFMETHOD)
  #error "You must define DEFMETHOD"
#endif

#if !defined(DEFPROPERTY_GET)
  #error "You must define DEFPROPERTY_GET"
#endif

#if !defined(DEFPROPERTY_SET)
  #error "You must define DEFPROPERTY_SET"
#endif

DEFMETHOD(Float32, Public, Boolean, IsInfinity)
DEFMETHOD(Float32, Public, Boolean, IsNaN)
DEFMETHOD(Float32, Public, Boolean, IsNegative)
DEFMETHOD(Float32, Public, Boolean, IsPositive)

DEFMETHOD(Float64, Public, Boolean, IsInfinity)
DEFMETHOD(Float64, Public, Boolean, IsNaN)
DEFMETHOD(Float64, Public, Boolean, IsNegative)
DEFMETHOD(Float64, Public, Boolean, IsPositive)

#if 0
  public interface Comparable<T> {
    public int CompareTo(T another);
  }
#endif
DEFMETHOD(Comparable, Public, Int32, CompareTo, *Ty_T, Q(another))

#if 0
  public interface Enumerator<T> {
    T Current { get; }
    bool HasMore();
    void MoveNext();
  }
#endif
DEFPROPERTY_GET(Enumerator, Public, T, Current)
DEFMETHOD(Enumerator, Public, Boolean, HasMore)
DEFMETHOD(Enumerator, Public, T, MoveNext)

#if 0
  public interface Enumerable<T> {
    Enumerator<T> GetEnumerator();
  }
#endif
DEFMETHOD(Enumerable, Public, CONSTRUCTED_CLASS_(Enumerator, T), GetEnumerator)

#if 0
  public interface Equatable<T> {
    public bool Equals(T another);
  }
#endif
DEFMETHOD(Equatable, Public, Boolean, Equals, *Ty_T, Q(another))

#if 0
  public interface Hashable<T> : Equatable<T> {
    int ComputeHashCode();
  }
#endif
DEFMETHOD(Hashable, Public, Int32, ComputeHashCode)

#if 0
  public final class String
      : Comparable<String>,
        Enumerable<Char>,
        Equatable<String>,
        Hashable<String> {
    public char this[int] { get; set; }
    public int Length { get; }
    public String ToString()
  }
#endif
DEFPROPERTY_GET(String, Public, Int32, Length)
DEFMETHOD(String, Public, Char, get_Item, *Ty_Int32, Q(index))
DEFMETHOD(String, Public, Void, set_Item, *Ty_Char, Q(value), *Ty_Int32, Q(index))
DEFMETHOD(String, Public, String, ToString)

#if 0
  public final class Object {
    public virtual Type GetType();
    public virtual String ToString();
  }
#endif
DEFMETHOD(Object, Public | Virtual, Type, GetType)
DEFMETHOD(Object, Public | Virtual, String, ToString)

#if 0
  public final class Array<T> {
    public static T[] Create(int dimension);
    public T this[int] { get; set; }
    public int Length { get; }
    public int Rank { get; }
  }
#endif
DEFMETHOD(Array, Public | Static, ARRAY_(T), Create, *Ty_Int32, Q(dimension))
DEFMETHOD(Array, Public, T, get_Item, *Ty_Int32, Q(index))
DEFMETHOD(Array, Public, Void, set_Item, *Ty_T, Q(value), *Ty_Int32, Q(index))
DEFPROPERTY_GET(Array, Public, Int32, Length)
DEFPROPERTY_GET(Array, Public, Int32, Rank)

#if 0
public final struct Nullable<T> where T : Value {
  private T value_;
  private T has_value_;
  public Nullable(T value) { has_value_ = true; value_ = value; }
  public Nullable() { has_value_ = false; }
  public bool HasValue() { get { return has_value_; } }
  public T Value { get { return has_value_ ? value_ : default(T); } }
  public static explicit operator T(Nullable<T> value);
  public static implicit operator Nullable<T>(T value);
}
#endif
DEFMETHOD(Nullable, Public, Void, .ctor, *Ty_T, Q(value))
DEFMETHOD(Nullable, Public, Void, .ctor)
DEFPROPERTY_GET(Nullable, Public, Boolean, HasValue)
DEFPROPERTY_GET(Nullable, Public, T, Value)

//DEFCONVERSION(Nullable, Public | Explicit, T, CONSTRUCTED_CLASS_(Nullable, T))
//DEFCONVERSION(Nullable, Public | Implicit, CONSTRUCTED_CLASS_(Nullable, T), T)

#if 0
public final struct ClosedCell<T> {
    T value;
    ClosedCell(T value) { this.value = value; }
#endif
DEFMETHOD(ClosedCell, Public, Void, .ctor, *Ty_T, Q(value))
DEFMETHOD(LiteralCell, Public, Void, .ctor, *Ty_T, Q(value))
DEFMETHOD(StackCell, Public, Void, .ctor, *Ty_T, Q(value))

#if 0
  public interface Disposable {
    void Dispose();
  }
#endif
DEFMETHOD(Disposable, Public, Void, Dispose)

#if 0
  public class Exception : Enumerator {
    public Exception(String message);
    public Exception(Exception innerException, String message);
    public String Message { get; }
    public String InnerException { get; }
  }
#endif
DEFMETHOD(Exception, Public, Void, .ctor, *Ty_String, Q(message))
DEFMETHOD(Exception, Public, Void, .ctor, *Ty_Exception, Q(innerException), *Ty_String, Q(message))
DEFPROPERTY_GET(Exception, Public, String, Message)
DEFPROPERTY_GET(Exception, Public, Exception, InnerException)

#if 0
  public class ArgumentOutOfRangeException : Exception {
    public ArgumentOutOfRangeException(String message);
    public ArgumentOutOfRangeException(Exception innerException, String message);
  }
#endif
DEFMETHOD(ArgumentOutOfRangeException, Public, Void, .ctor, *Ty_String, Q(message))
DEFMETHOD(ArgumentOutOfRangeException, Public, Void, .ctor, *Ty_Exception, Q(innerException), *Ty_String, Q(message))

// Note: We don't need to define Console class here. We have Console
// class here for testing.
#if 0
  public static class Console {
    public void Write(String string);
    public void WriteLine();
    public void WriteLine(String string);
    public void WriteLine(String format, Object[] paramv);
  }
#endif
DEFMETHOD(Console, Public | Static, Void, Write, *Ty_String, Q(string))
DEFMETHOD(Console, Public | Static, Void, WriteLine)
DEFMETHOD(Console, Public | Static, Void, WriteLine, *Ty_String, Q(string))
DEFMETHOD(Console, Public | Static, Void, WriteLine, *Ty_String, Q(format), ARRAY_(Object), Q(paramv))

#undef DEFMETHOD
#undef DEFPROPERTY_GET
#undef DEFPROPERTY_SET
