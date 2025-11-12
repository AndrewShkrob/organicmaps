package app.organicmaps.sdk.widget.speed.internal;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.Typeface;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import androidx.annotation.ColorInt;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.RestrictTo;

@RestrictTo(RestrictTo.Scope.LIBRARY)
public class SpeedLimitView extends View
{
  public interface OnTextSizeChangedListener
  {
    void onTextSizeChanged(float textSize);
  }

  // "8" is the widest digit, so we use three of them to calculate the maximum text size
  private static final String CONFIGURATION_STRING = "888";

  private interface DefaultValues
  {
    @ColorInt
    int BACKGROUND_COLOR = Color.WHITE;
    @ColorInt
    int BORDER_COLOR = Color.RED;
    @ColorInt
    int TEXT_COLOR = Color.BLACK;

    float BORDER_WIDTH_RATIO = 0.1f;
  }

  @Nullable
  private OnTextSizeChangedListener mOnTextSizeChangedListener;

  @NonNull
  private final Paint mSignBackgroundPaint;
  @NonNull
  private final Paint mSignBorderPaint;
  @NonNull
  private final Paint mTextPaint;

  private float mWidth;
  private float mHeight;
  private float mBackgroundRadius;
  private float mBorderRadius;
  private float mBorderWidth;

  @NonNull
  private String mSpeedLimit = "0";

  public SpeedLimitView(Context context)
  {
    this(context, null);
  }

  public SpeedLimitView(Context context, @Nullable AttributeSet attrs)
  {
    this(context, attrs, 0);
  }

  public SpeedLimitView(Context context, @Nullable AttributeSet attrs, int defStyleAttr)
  {
    this(context, attrs, defStyleAttr, 0);
  }

  public SpeedLimitView(Context context, @Nullable AttributeSet attrs, int defStyleAttr, int defStyleRes)
  {
    super(context, attrs, defStyleAttr, defStyleRes);

    mSignBackgroundPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
    mSignBackgroundPaint.setColor(DefaultValues.BACKGROUND_COLOR);

    mSignBorderPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
    mSignBorderPaint.setColor(DefaultValues.BORDER_COLOR);
    mSignBorderPaint.setStrokeWidth(mBorderWidth);
    mSignBorderPaint.setStyle(Paint.Style.STROKE);

    mTextPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
    mTextPaint.setColor(DefaultValues.TEXT_COLOR);
    mTextPaint.setTextAlign(Paint.Align.CENTER);
    mTextPaint.setTypeface(Typeface.create(Typeface.DEFAULT, Typeface.BOLD));

    if (isInEditMode())
      mSpeedLimit = "60";
  }

  public void setSpeedLimit(@NonNull String speedLimit)
  {
    if (mSpeedLimit.equals(speedLimit))
      return;

    mSpeedLimit = speedLimit;
    invalidate();
  }

  @Override
  public void setBackgroundColor(@ColorInt int color)
  {
    mSignBackgroundPaint.setColor(color);
    invalidate();
  }

  public void setBorderColor(@ColorInt int color)
  {
    mSignBorderPaint.setColor(color);
    invalidate();
  }

  public void setTextColor(@ColorInt int color)
  {
    mTextPaint.setColor(color);
    invalidate();
  }

  public void setOnTextSizeChangedListener(@Nullable OnTextSizeChangedListener listener)
  {
    mOnTextSizeChangedListener = listener;
  }

  public void setSize(int size)
  {
    final ViewGroup.LayoutParams params = getLayoutParams();
    params.width = size;
    params.height = size;
    setLayoutParams(params);
    onSizeChanged(size, size, 0, 0);
  }

  public float getMaxTextWidth()
  {
    final Rect textBounds = new Rect();
    mTextPaint.getTextBounds(CONFIGURATION_STRING, 0, CONFIGURATION_STRING.length(), textBounds);
    return textBounds.width();
  }

  @Override
  protected void onDraw(@NonNull Canvas canvas)
  {
    super.onDraw(canvas);

    final float cx = mWidth / 2;
    final float cy = mHeight / 2;

    drawSign(canvas, cx, cy);
    drawText(canvas, cx, cy);
  }

  private void drawSign(@NonNull Canvas canvas, float cx, float cy)
  {
    mSignBorderPaint.setStrokeWidth(mBorderWidth);

    canvas.drawCircle(cx, cy, (mBackgroundRadius + mBorderRadius) / 2, mSignBackgroundPaint);
    canvas.drawCircle(cx, cy, mBorderRadius, mSignBorderPaint);
  }

  private void drawText(@NonNull Canvas canvas, float cx, float cy)
  {
    final Rect textBounds = new Rect();
    mTextPaint.getTextBounds(mSpeedLimit, 0, mSpeedLimit.length(), textBounds);
    final float textY = cy - textBounds.exactCenterY();
    canvas.drawText(mSpeedLimit, cx, textY, mTextPaint);
  }

  @Override
  public boolean onTouchEvent(@NonNull MotionEvent event)
  {
    final float cx = mWidth / 2;
    final float cy = mHeight / 2;
    if (Math.pow(event.getX() - cx, 2) + Math.pow(event.getY() - cy, 2) <= Math.pow(mBackgroundRadius, 2))
    {
      performClick();
      return true;
    }
    return false;
  }

  @Override
  public boolean performClick()
  {
    super.performClick();
    return false;
  }

  @Override
  protected void onSizeChanged(int w, int h, int oldw, int oldh)
  {
    super.onSizeChanged(w, h, oldw, oldh);

    final float paddingX = (float) (getPaddingLeft() + getPaddingRight());
    final float paddingY = (float) (getPaddingTop() + getPaddingBottom());

    final float newWidth = (float) w - paddingX;
    final float newHeight = (float) h - paddingY;

    if (mWidth == newWidth && mHeight == newHeight)
      return;

    mWidth = (float) w - paddingX;
    mHeight = (float) h - paddingY;
    mBackgroundRadius = Math.min(mWidth, mHeight) / 2;
    mBorderWidth = mBackgroundRadius * 2 * DefaultValues.BORDER_WIDTH_RATIO;
    mBorderRadius = mBackgroundRadius - mBorderWidth / 2;
    configureTextSize();
  }

  // Apply binary search to determine the optimal text size that fits within the circular boundary.
  private void configureTextSize()
  {
    final String text = CONFIGURATION_STRING;
    final float textRadius = mBorderRadius - mBorderWidth;
    final float textMaxSize = 2 * textRadius;
    final float textMaxSizeSquared = (float) Math.pow(textMaxSize, 2);

    float lowerBound = 0;
    float upperBound = textMaxSize;
    float textSize = textMaxSize;
    final Rect textBounds = new Rect();

    while (lowerBound <= upperBound)
    {
      textSize = (lowerBound + upperBound) / 2;
      mTextPaint.setTextSize(textSize);
      mTextPaint.getTextBounds(text, 0, text.length(), textBounds);

      if (Math.pow(textBounds.width(), 2) + Math.pow(textBounds.height(), 2) <= textMaxSizeSquared)
        lowerBound = textSize + 1;
      else
        upperBound = textSize - 1;
    }

    mTextPaint.setTextSize(Math.max(1, textSize));
    if (mOnTextSizeChangedListener != null)
      mOnTextSizeChangedListener.onTextSizeChanged(mTextPaint.getTextSize());
  }
}
