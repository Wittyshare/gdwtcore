cmake_minimum_required(VERSION 2.8.5)

# Cette librairie contient des widgets supplémentaires pour wt

SET(DEPLOY_RESOURCES ${GDWT_PREFIX}/share/Wt/resources/ CACHE PATH "These library resources is deployed under the wt resources")
SET(DEPLOYROOT /var/www/wt/resources CACHE PATH "These library is deployed under the wt resources")

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${GDCORE_PREFIX})

SET ( APPNAME_SRCS 
      src/WAsterisk/gdMeetingView.cpp
      src/WLdap/gdLdapTree.cpp
      src/gdWtThread.cpp
      src/gdComboBoxDelegate.cpp
      src/gdSpinBoxDelegate.cpp
      src/gdDoubleSpinBoxDelegate.cpp
      src/gdImageDelegate.cpp
      src/gdTextImageDelegate.cpp
      src/gdLineEditDelegate.cpp
      src/gdNumericDelegate.cpp
      src/gdWTextDelegate.cpp
      src/gdCsvModel.C
      src/gdViewToPdf.cpp
      src/gdViewToOds.cpp
      src/gdTreeViewPdfRes.cpp
      src/gdStdItem.cpp
      src/gdStdModel.cpp
      src/gdSqlModel.cpp
      src/gdProxyModel.cpp
      src/gdSqlTreeView.cpp
      src/gdSqlTableView.cpp
      src/gdSqlComboBox.cpp
      src/gdSqlComboBoxDelegate.cpp
      src/WPanelList.C
      src/dlgHourGlass.cpp
      src/gdApplication.cpp
      src/gdToolbar.cpp
      src/gdWDialog.cpp
      src/gdWSplashDialog.cpp
      src/gdContainerBar.cpp
      src/gdWFileView.cpp
      src/gdWFileDialog.cpp
      src/gdSqlContainerBar.cpp
      src/gdSqlSuggestionPopup.cpp
      src/gdWFileResourceDelete.cpp
      src/gdWPrintersDialog.cpp
      src/gdWColorDialog.cpp
      src/gdWColor.cpp
      src/gdWSearch.cpp
      src/gdWLogon.cpp
      src/gdWForm.cpp
      src/gdFHtmlEditor.cpp
    )

INCLUDE(${CMAKE_ROOT}/Modules/gdcmake/Find_gdcore.cmake)
INCLUDE(${CMAKE_HOME_DIRECTORY}/cmake/Find_gdwt.cmake)
INCLUDE(${CMAKE_HOME_DIRECTORY}/cmake/Find_vmime.cmake)

INCLUDE_DIRECTORIES(
                    ./
                    ${GDCORE_INCLUDE_DIRS}
                    ${GDWT_INCLUDE_DIR}
)

INCLUDE(${CMAKE_ROOT}/Modules/gdcmake/install_libs.cmake)

TARGET_LINK_LIBRARIES(
    gdwtcore ${GDCORE_LIBRARY} ${GDWT_LIBRARY} ${VMIME_LIBRARIES}
)

