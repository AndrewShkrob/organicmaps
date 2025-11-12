package app.organicmaps.car.renderer;

import android.app.Presentation;
import android.graphics.Rect;
import android.hardware.display.DisplayManager;
import android.hardware.display.VirtualDisplay;
import android.view.Gravity;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewParent;
import android.widget.FrameLayout;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.RequiresApi;
import androidx.car.app.CarContext;
import androidx.car.app.SurfaceContainer;
import app.organicmaps.R;
import app.organicmaps.sdk.MapController;
import app.organicmaps.sdk.settings.UnitLocale;
import app.organicmaps.sdk.util.log.Logger;
import app.organicmaps.sdk.widget.speed.SpeedView;
import app.organicmaps.sdk.widget.speed.SpeedViewManager;

@RequiresApi(23)
class SurfaceCallback extends SurfaceCallbackBase
{
  private static final String TAG = SurfaceCallback.class.getSimpleName();

  private static final int SPEED_LIMIT_VIEW_SIZE_DP = 80;

  private static final String VIRTUAL_DISPLAY_NAME = "OM_Android_Auto_Display";

  @NonNull
  private final MapController mMapController;
  @Nullable
  private FrameLayout mSpeedLimitContainer;
  @Nullable
  private SpeedViewManager mSpeedViewManager;

  @Nullable
  private VirtualDisplay mVirtualDisplay;
  @Nullable
  private Presentation mPresentation;

  public SurfaceCallback(@NonNull CarContext carContext, @NonNull MapController mapController)
  {
    super(carContext);
    mMapController = mapController;
    mMapController.getView().getHolder().addCallback(new SurfaceHolder.Callback() {
      @Override
      public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height)
      {
        mMapController.updateMyPositionRoutingOffset(0);
      }
      @Override
      public void surfaceCreated(@NonNull SurfaceHolder holder)
      {
        mMapController.updateMyPositionRoutingOffset(0);
      }
      @Override
      public void surfaceDestroyed(@NonNull SurfaceHolder holder)
      {}
    });
    initSpeedLimitView();
  }

  @Override
  public void onSurfaceAvailable(@NonNull SurfaceContainer surfaceContainer)
  {
    Logger.d(TAG, "Surface available " + surfaceContainer);

    mVirtualDisplay =
        mCarContext.getSystemService(DisplayManager.class)
            .createVirtualDisplay(VIRTUAL_DISPLAY_NAME, surfaceContainer.getWidth(), surfaceContainer.getHeight(),
                                  surfaceContainer.getDpi(), surfaceContainer.getSurface(),
                                  DisplayManager.VIRTUAL_DISPLAY_FLAG_OWN_CONTENT_ONLY);
    mPresentation = new Presentation(mCarContext, mVirtualDisplay.getDisplay());
    mPresentation.setContentView(prepareViewForPresentation(mMapController.getView()));
    mPresentation.show();
  }

  @Override
  public void onVisibleAreaChanged(@NonNull Rect visibleArea)
  {
    super.onVisibleAreaChanged(visibleArea);

    assert mSpeedLimitContainer != null : "mSpeedLimitContainer must be initialized";
    mSpeedLimitContainer.setLayoutParams(getSpeedLimitContainerLayoutParams());
  }

  @Override
  public void onSurfaceDestroyed(@NonNull SurfaceContainer surfaceContainer)
  {
    Logger.d(TAG, "Surface destroyed");
    if (mPresentation != null)
      mPresentation.dismiss();
    if (mVirtualDisplay != null)
      mVirtualDisplay.release();
  }

  @NonNull
  SpeedViewManager getSpeedViewManager()
  {
    assert mSpeedViewManager != null : "mSpeedLimitContainer must be initialized";
    return mSpeedViewManager;
  }

  void stopPresenting()
  {
    if (mPresentation != null)
      mPresentation.dismiss();
  }

  void startPresenting()
  {
    if (mPresentation != null)
      mPresentation.show();
  }

  @NonNull
  private View prepareViewForPresentation(@NonNull View view)
  {
    final ViewParent parent = view.getParent();
    if (parent instanceof ViewGroup)
      ((ViewGroup) parent).removeView(view);

    final FrameLayout container = new FrameLayout(mCarContext);
    container.addView(
        view, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
    initSpeedLimitView();
    container.addView(mSpeedLimitContainer);

    return container;
  }

  private void initSpeedLimitView()
  {
    mSpeedLimitContainer = new FrameLayout(mCarContext);
    mSpeedLimitContainer.setLayoutParams(getSpeedLimitContainerLayoutParams());

    final SpeedView speedView = new SpeedView(mCarContext, null, 0, R.style.MwmWidget_SpeedLimit_AndroidAuto);
    mSpeedViewManager = new SpeedViewManager(speedView);
    speedView.setSpeedUnits(UnitLocale.getUnits() == UnitLocale.UNITS_METRIC ? R.string.kilometers_per_hour
                                                                             : R.string.miles_per_hour);

    final FrameLayout.LayoutParams speedLimitLayoutParams =
        new FrameLayout.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
    speedLimitLayoutParams.gravity = Gravity.END | Gravity.BOTTOM;
    mSpeedLimitContainer.addView(speedView, speedLimitLayoutParams);
  }

  @NonNull
  private ViewGroup.LayoutParams getSpeedLimitContainerLayoutParams()
  {
    final FrameLayout.LayoutParams layoutParams =
        new FrameLayout.LayoutParams(mVisibleArea.right - mVisibleArea.left, // width
                                     mVisibleArea.bottom - mVisibleArea.top // height
        );
    layoutParams.leftMargin = mVisibleArea.left;
    layoutParams.topMargin = mVisibleArea.top;
    layoutParams.gravity = Gravity.NO_GRAVITY;
    return layoutParams;
  }
}
